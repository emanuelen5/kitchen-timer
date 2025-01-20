#include <unity.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <vector>

#include "bootloader_sm.h"
#include "state.h"

// Test doubles
struct
{
    struct
    {
        uint8_t received_byte;
        int status;
    } receive_and_checksum;

    std::vector<uint8_t> sent_data;
} test_state;

void set_counter(uint8_t count)
{
    (void)count;
}

uint16_t dummy_checksum(uint16_t crc, int8_t byte)
{
    return crc + byte;
}

int receive_and_checksum(uint8_t *byte, uint16_t *crc)
{
    *byte = test_state.receive_and_checksum.received_byte;
    *crc = dummy_checksum(*crc, *byte);
    return test_state.receive_and_checksum.status;
}

void set_receive_and_checksum_return_value(uint8_t received_byte, int status)
{
    test_state.receive_and_checksum.received_byte = received_byte;
    test_state.receive_and_checksum.status = status;
}

void write_page(const uint8_t page_offset, const uint8_t *program_buffer)
{
    (void)page_offset;
    (void)program_buffer;
}

void increment_counter()
{
    return;
}

uint8_t *read_signature(unsigned char *)
{
    return NULL;
}

void UART_send(uint8_t data)
{
    test_state.sent_data.push_back(data);
}

uint16_t send_and_checksum(uint8_t byte, uint16_t crc)
{
    UART_send(byte);
    return dummy_checksum(crc, byte);
}

state_machine_t sm;

void setUp(void)
{
    test_state = {};
    test_state.sent_data.clear();
    sm = {};
}

void tearDown(void)
{
}

void send_to_microcontroller_and_update_its_state_machine(uint8_t byte)
{
    set_receive_and_checksum_return_value(byte, resp_ok);
    step_state_machine(sm);
}

void test_init_steps_to_wait_for_programmer(void)
{
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_INIT);
    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_WAIT_FOR_PROGRAMMER);
}

void test_wait_for_programmer_keeps_waiting_if_data_wasnt_start_byte(void)
{
    const uint8_t NOT_START_BYTE = 0x01;
    sm.state = STATE_WAIT_FOR_PROGRAMMER;
    send_to_microcontroller_and_update_its_state_machine(NOT_START_BYTE);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_WAIT_FOR_START_BYTE);
    TEST_ASSERT_EQUAL(NOT_START_BYTE, sm.calculated_checksum);
}

void test_wait_for_programmer_wants_command_if_there_was_start_byte(void)
{
    sm.state = STATE_WAIT_FOR_PROGRAMMER;
    send_to_microcontroller_and_update_its_state_machine(START_BYTE);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_COMMAND);
    TEST_ASSERT_NOT_EQUAL(0, sm.calculated_checksum);
}

void test_wait_for_programmer_times_out(void)
{
    const uint8_t dontcare = 0;
    sm.state = STATE_WAIT_FOR_PROGRAMMER;
    set_receive_and_checksum_return_value(dontcare, resp_timeout);
    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_EXIT);
}

void test_write_page(void)
{
    sm.state = STATE_COMMAND;
    send_to_microcontroller_and_update_its_state_machine(COMMAND_WRITE_PAGE);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_LENGTH);
    send_to_microcontroller_and_update_its_state_machine(SPM_PAGESIZE);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    for (int i = 0; i < SPM_PAGESIZE; i++)
    {
        send_to_microcontroller_and_update_its_state_machine(0x01);
        char message[50] = {0};
        snprintf(message, sizeof(message), "After %d bytes", i);
        TEST_ASSERT_BOOTLOADER_IN_STATE_MESSAGE(STATE_DATA, message);
    }
    const uint16_t checksum = COMMAND_WRITE_PAGE + SPM_PAGESIZE + 0x01 * SPM_PAGESIZE;
    TEST_ASSERT_EQUAL(checksum, sm.calculated_checksum);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    // make the total checksum 0, otherwise the microcontroller will NAK
    send_to_microcontroller_and_update_its_state_machine(-2 * SPM_PAGESIZE - COMMAND_WRITE_PAGE);
    TEST_ASSERT_EQUAL(0, sm.calculated_checksum);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    send_to_microcontroller_and_update_its_state_machine(0);
    TEST_ASSERT_EQUAL(0, sm.calculated_checksum);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_CHECK_CHECKSUM);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_RUN_COMMAND);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_RETURN_STATUS);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_WAIT_FOR_START_BYTE);

    TEST_ASSERT_EQUAL(9, test_state.sent_data.size());
    const uint8_t length = 4;
    const uint8_t chksum = 7;
    uint8_t arr[] = {START_BYTE, COMMAND_WRITE_PAGE, length, resp_ok, 0x00, 0x00, 0x00, 0x00, chksum};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(arr, test_state.sent_data.data(), 9);
}

void test_read_signature(void)
{
    sm.state = STATE_COMMAND;
    send_to_microcontroller_and_update_its_state_machine(COMMAND_READ_SIGNATURE);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_LENGTH);
    send_to_microcontroller_and_update_its_state_machine(0);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    const uint16_t checksum = COMMAND_READ_SIGNATURE;
    TEST_ASSERT_EQUAL(checksum, sm.calculated_checksum);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    // make the total checksum 0, otherwise the microcontroller will NAK
    send_to_microcontroller_and_update_its_state_machine(-COMMAND_READ_SIGNATURE);
    TEST_ASSERT_EQUAL(0, sm.calculated_checksum);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    send_to_microcontroller_and_update_its_state_machine(0);
    TEST_ASSERT_EQUAL(0, sm.calculated_checksum);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_CHECK_CHECKSUM);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_RUN_COMMAND);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_RETURN_STATUS);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_WAIT_FOR_START_BYTE);

    TEST_ASSERT_EQUAL(9, test_state.sent_data.size());
    const uint8_t length = 4;
    const uint8_t chksum = 8;
    uint8_t arr[] = {START_BYTE, COMMAND_READ_SIGNATURE, length, resp_ok, 0x00, 0x00, 0x00, 0x00, chksum};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(arr, test_state.sent_data.data(), 9);
}

void test_boot(void)
{
    sm.state = STATE_COMMAND;
    send_to_microcontroller_and_update_its_state_machine(COMMAND_BOOT);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_LENGTH);

    send_to_microcontroller_and_update_its_state_machine(0);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    const uint16_t checksum = COMMAND_BOOT;
    TEST_ASSERT_EQUAL(checksum, sm.calculated_checksum);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    // make the total checksum 0, otherwise the microcontroller will NAK
    send_to_microcontroller_and_update_its_state_machine(-COMMAND_BOOT);
    TEST_ASSERT_EQUAL(0, sm.calculated_checksum);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    send_to_microcontroller_and_update_its_state_machine(0);
    TEST_ASSERT_EQUAL(0, sm.calculated_checksum);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_CHECK_CHECKSUM);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_RUN_COMMAND);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_RETURN_STATUS);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_EXIT);

    TEST_ASSERT_EQUAL(9, test_state.sent_data.size());
    const uint8_t length = 4;
    const uint8_t chksum = 9;
    uint8_t arr[] = {START_BYTE, COMMAND_BOOT, length, resp_ok, 0x00, 0x00, 0x00, 0x00, chksum};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(arr, test_state.sent_data.data(), 9);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_init_steps_to_wait_for_programmer);
    RUN_TEST(test_wait_for_programmer_keeps_waiting_if_data_wasnt_start_byte);
    RUN_TEST(test_wait_for_programmer_wants_command_if_there_was_start_byte);
    RUN_TEST(test_wait_for_programmer_times_out);
    RUN_TEST(test_write_page);
    RUN_TEST(test_read_signature);
    RUN_TEST(test_boot);

    UNITY_END();
}