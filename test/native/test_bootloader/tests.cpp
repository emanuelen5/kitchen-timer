#include <unity.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <vector>
#include <stdexcept>

#include "bootloader_sm.h"
#include "state.h"
#include "command.h"

enum
{
    resp_ok = 0,
    resp_timeout = 1,
};

// Test doubles
struct
{
    struct
    {
        uint8_t received_byte;
        bool received_byte_set;
        int status;
        int send_count;
        uint16_t checksum;
    } receive_and_checksum;

    std::vector<uint8_t> sent_data;

    struct
    {
        int call_count = 0;
        uint16_t page_offset = 0;
        uint8_t data[SPM_PAGESIZE] = {0};
    } read_page;

    struct
    {
        int call_count = 0;
        uint16_t page_offset = 0;
        uint8_t data[SPM_PAGESIZE] = {0};
    } write_page;
} test_state;

class ResponseBreakdown
{
public:
    ResponseBreakdown(const std::vector<uint8_t> &data)
    {
        if (data.size() < 9)
            throw std::runtime_error("Data too short to parse response");

        start_byte = data[0];
        status = data[1];
        length = data[2];

        this->data.resize(length);
        for (size_t i = 0; i < this->data.size(); ++i)
        {
            this->data[i] = data[i + 3];
        }
        checksum = data[length + 4] << 8 | data[length + 3];
    }

    uint8_t start_byte;
    uint8_t status;
    uint8_t length;
    std::vector<uint8_t> data;
    uint16_t checksum;

    void print() const
    {
        printf("Response Breakdown:\n");
        printf("  Start Byte: 0x%02X\n", start_byte);
        printf("  status: 0x%02X (%s)\n", status, response_to_string(status));
        printf("  Length: %d\n", length);
        printf("  Data: ");
        for (const auto &byte : data)
            printf("0x%02X ", byte);
        printf("\n");
        printf("  Checksum: 0x%04X\n", checksum);
    }
};

void set_counter(uint8_t count)
{
    (void)count;
}

uint16_t dummy_checksum(uint16_t crc, int8_t byte)
{
    return (crc + byte) & 0xff; // Keep within one byte so we can always zero it out
}

uint8_t UART_receive(void)
{
    if (!test_state.receive_and_checksum.received_byte_set)
        TEST_FAIL_MESSAGE("UART_receive called but no byte was set for reception");

    test_state.receive_and_checksum.received_byte_set = false;
    return test_state.receive_and_checksum.received_byte;
}

void set_receive_and_checksum_return_value(uint8_t received_byte, int status)
{
    test_state.receive_and_checksum.received_byte_set = true;
    test_state.receive_and_checksum.received_byte = received_byte;
    test_state.receive_and_checksum.status = status;
    test_state.receive_and_checksum.checksum = dummy_checksum(received_byte, test_state.receive_and_checksum.checksum);
}

void write_page(const uint16_t page_offset, const uint8_t *program_buffer)
{
    test_state.write_page.call_count++;
    test_state.write_page.page_offset = page_offset;
    memcpy(test_state.write_page.data, program_buffer, SPM_PAGESIZE);
}

void read_page(const uint16_t page_offset, uint8_t *program_buffer)
{
    test_state.read_page.call_count++;
    test_state.read_page.page_offset = page_offset;
    memcpy(program_buffer, test_state.read_page.data, SPM_PAGESIZE);
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

int UART_receive_with_timeout(uint8_t *data)
{
    if (!test_state.receive_and_checksum.received_byte_set)
        TEST_FAIL_MESSAGE("UART_receive_with_timeout called but no byte was set for reception");

    if (test_state.receive_and_checksum.status)
        return resp_timeout;

    test_state.receive_and_checksum.received_byte_set = false;
    *data = test_state.receive_and_checksum.received_byte;
    return 0; // Successful reception
}

uint16_t checksum(uint8_t byte, uint16_t crc)
{
    return dummy_checksum(crc, byte);
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
    test_state.receive_and_checksum.checksum = 0xffff;
    sm = {};
}

void tearDown(void)
{
}

void send_to_microcontroller_and_update_its_state_machine(uint8_t byte)
{
    test_state.receive_and_checksum.send_count++;
    set_receive_and_checksum_return_value(byte, resp_ok);
    printf("[%s] Sending byte(%d): 0x%02X (checksum: 0x%02X)\n",
           state_to_str(sm.state),
           test_state.receive_and_checksum.send_count,
           byte,
           test_state.receive_and_checksum.checksum);
    step_state_machine(sm);
}

void send_end_of_packet()
{
    int8_t zeroing_checksum = -test_state.receive_and_checksum.checksum;
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);
    send_to_microcontroller_and_update_its_state_machine(zeroing_checksum);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);
    send_to_microcontroller_and_update_its_state_machine(0);
    test_state.receive_and_checksum.checksum = 0xffff; // Reset checksum for next packet
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
}

void test_wait_for_programmer_wants_command_if_there_was_start_byte(void)
{
    sm.state = STATE_WAIT_FOR_PROGRAMMER;
    send_to_microcontroller_and_update_its_state_machine(START_BYTE);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_COMMAND);
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
    sm.state = STATE_WAIT_FOR_START_BYTE;
    send_to_microcontroller_and_update_its_state_machine(START_BYTE);
    send_to_microcontroller_and_update_its_state_machine(COMMAND_WRITE_PAGE);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_LENGTH);
    send_to_microcontroller_and_update_its_state_machine(2 + SPM_PAGESIZE);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    uint16_t page_offset = 0x1234;
    send_to_microcontroller_and_update_its_state_machine(page_offset & 0xff);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);
    send_to_microcontroller_and_update_its_state_machine(page_offset >> 8);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    for (int i = 0; i < SPM_PAGESIZE; i++)
    {
        send_to_microcontroller_and_update_its_state_machine(0x01);
        char message[50] = {0};
        snprintf(message, sizeof(message), "After %d bytes", i);
        printf("[%s] %s\n", state_to_str(sm.state), message);
        TEST_ASSERT_BOOTLOADER_IN_STATE_MESSAGE(STATE_DATA, message);
    }
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    send_end_of_packet();
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_CHECK_CHECKSUM);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_RUN_COMMAND);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_RETURN_STATUS);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_WAIT_FOR_START_BYTE);

    TEST_ASSERT_EQUAL(9, test_state.sent_data.size());

    ResponseBreakdown response(test_state.sent_data);
    response.print();
    TEST_ASSERT_EQUAL_MESSAGE(resp_ack, response.status, "Status");
    TEST_ASSERT_EQUAL_MESSAGE(4, response.length, "Length");
    TEST_ASSERT_EQUAL_MESSAGE(4, response.data.size(), "Data size");
    uint8_t arr[] = {(uint8_t)(page_offset & 0xff), (uint8_t)(page_offset >> 8), 0x00, 0x00};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(arr, response.data.data(), 4);
}

void test_read_page(void)
{
    sm.state = STATE_WAIT_FOR_START_BYTE;
    send_to_microcontroller_and_update_its_state_machine(START_BYTE);
    send_to_microcontroller_and_update_its_state_machine(COMMAND_READ_PAGE);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_LENGTH);
    send_to_microcontroller_and_update_its_state_machine(2);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    const uint16_t page_offset = 0x1234;
    send_to_microcontroller_and_update_its_state_machine(page_offset & 0xff);
    send_to_microcontroller_and_update_its_state_machine(page_offset >> 8);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    send_end_of_packet();
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_CHECK_CHECKSUM);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_RUN_COMMAND);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_READ_PAGE);

    step_state_machine(sm); // STATE_WAIT_FOR_START_BYTE

    TEST_ASSERT_EQUAL(7 + SPM_PAGESIZE, test_state.sent_data.size());

    ResponseBreakdown response(test_state.sent_data);
    response.print();
    TEST_ASSERT_EQUAL_MESSAGE(resp_ack, response.status, "Status");
    TEST_ASSERT_EQUAL_MESSAGE(SPM_PAGESIZE + 2, response.length, "Length");
}

void test_read_signature(void)
{
    sm.state = STATE_WAIT_FOR_START_BYTE;
    send_to_microcontroller_and_update_its_state_machine(START_BYTE);
    send_to_microcontroller_and_update_its_state_machine(COMMAND_READ_SIGNATURE);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_LENGTH);
    send_to_microcontroller_and_update_its_state_machine(0);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    send_end_of_packet();
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_CHECK_CHECKSUM);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_RUN_COMMAND);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_RETURN_STATUS);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_WAIT_FOR_START_BYTE);

    TEST_ASSERT_EQUAL(9, test_state.sent_data.size());

    ResponseBreakdown response(test_state.sent_data);
    response.print();
    TEST_ASSERT_EQUAL_MESSAGE(resp_ack, response.status, "Status");
    TEST_ASSERT_EQUAL_MESSAGE(4, response.length, "Length");
    TEST_ASSERT_EQUAL_MESSAGE(4, response.data.size(), "Data size");
    uint8_t arr[] = {0x00, 0x00, 0x00, 0x00};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(arr, response.data.data(), 4);
}

void test_boot(void)
{
    sm.state = STATE_WAIT_FOR_START_BYTE;
    send_to_microcontroller_and_update_its_state_machine(START_BYTE);
    send_to_microcontroller_and_update_its_state_machine(COMMAND_BOOT);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_LENGTH);

    send_to_microcontroller_and_update_its_state_machine(0);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    send_end_of_packet();
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_CHECK_CHECKSUM);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_RUN_COMMAND);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_RETURN_STATUS);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_EXIT);

    TEST_ASSERT_EQUAL(9, test_state.sent_data.size());

    ResponseBreakdown response(test_state.sent_data);
    response.print();
    TEST_ASSERT_EQUAL_MESSAGE(resp_ack, response.status, "Status");
    TEST_ASSERT_EQUAL_MESSAGE(4, response.length, "Length");
    TEST_ASSERT_EQUAL_MESSAGE(4, response.data.size(), "Data size");
    uint8_t arr[] = {0x00, 0x00, 0x00, 0x00};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(arr, response.data.data(), 4);
}

void test_invalid_command(void)
{
    sm.state = STATE_WAIT_FOR_START_BYTE;
    send_to_microcontroller_and_update_its_state_machine(START_BYTE);
    const uint8_t INVALID_COMMAND = 0xFF;
    send_to_microcontroller_and_update_its_state_machine(INVALID_COMMAND);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_LENGTH);
    send_to_microcontroller_and_update_its_state_machine(0);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    send_end_of_packet();
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_CHECK_CHECKSUM);

    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_RUN_COMMAND);
    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_RETURN_STATUS);
    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_WAIT_FOR_START_BYTE);

    // Should send a NAK or error response
    TEST_ASSERT_EQUAL(9, test_state.sent_data.size());

    ResponseBreakdown response(test_state.sent_data);
    response.print();
    TEST_ASSERT_EQUAL_MESSAGE(resp_data_unknown_command, response.status, "Status");
    TEST_ASSERT_EQUAL_MESSAGE(4, response.length, "Length");
    TEST_ASSERT_EQUAL_MESSAGE(4, response.data.size(), "Data size");
    uint8_t arr[] = {0x00, 0x00, 0x00, 0x00};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(arr, response.data.data(), 4);
}

void test_checksum_error(void)
{
    sm.state = STATE_WAIT_FOR_START_BYTE;
    send_to_microcontroller_and_update_its_state_machine(START_BYTE);
    send_to_microcontroller_and_update_its_state_machine(COMMAND_WRITE_PAGE);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_LENGTH);
    send_to_microcontroller_and_update_its_state_machine(2 + SPM_PAGESIZE);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);
    const uint16_t page_offset = 0x1234;
    send_to_microcontroller_and_update_its_state_machine(page_offset & 0xff);
    send_to_microcontroller_and_update_its_state_machine(page_offset >> 8);
    for (int i = 0; i < SPM_PAGESIZE; i++)
        send_to_microcontroller_and_update_its_state_machine(0x01);

    // Do NOT make checksum zero, so it should fail
    send_to_microcontroller_and_update_its_state_machine(0);
    send_to_microcontroller_and_update_its_state_machine(0);

    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_CHECK_CHECKSUM);
    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_WRONG_CHECKSUM);
    step_state_machine(sm);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_RETURN_STATUS);
    step_state_machine(sm);

    TEST_ASSERT_EQUAL(9, test_state.sent_data.size());

    ResponseBreakdown response(test_state.sent_data);
    response.print();
    TEST_ASSERT_EQUAL_MESSAGE(resp_nak, response.status, "Status");
}

void test_timeout_in_data_state(void)
{
    sm.state = STATE_WAIT_FOR_START_BYTE;
    send_to_microcontroller_and_update_its_state_machine(START_BYTE);
    send_to_microcontroller_and_update_its_state_machine(COMMAND_WRITE_PAGE);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_LENGTH);
    send_to_microcontroller_and_update_its_state_machine(SPM_PAGESIZE);
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);

    // Simulate timeout
    set_receive_and_checksum_return_value(0, resp_timeout);
    step_state_machine(sm);

    // Keep waiting for data
    TEST_ASSERT_BOOTLOADER_IN_STATE(STATE_DATA);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_init_steps_to_wait_for_programmer);
    RUN_TEST(test_wait_for_programmer_keeps_waiting_if_data_wasnt_start_byte);
    RUN_TEST(test_wait_for_programmer_wants_command_if_there_was_start_byte);
    RUN_TEST(test_wait_for_programmer_times_out);
    RUN_TEST(test_write_page);
    RUN_TEST(test_read_page);
    RUN_TEST(test_read_signature);
    RUN_TEST(test_boot);
    RUN_TEST(test_invalid_command);
    RUN_TEST(test_checksum_error);
    RUN_TEST(test_timeout_in_data_state);

    UNITY_END();
}