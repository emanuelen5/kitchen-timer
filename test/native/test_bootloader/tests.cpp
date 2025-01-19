#include <unity.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "bootloader_sm.h"

// Test doubles
struct
{
    struct
    {
        uint8_t received_byte;
        int status;
    } receive_and_checksum;
} test_state;

void set_counter(uint8_t count)
{
    (void)count;
}

uint16_t dummy_checksum(uint16_t crc, uint8_t byte)
{
    (void)byte;
    return crc + 1;
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

void send_response(packet_t &packet)
{
    (void)packet;
}

state_machine_t sm;

void setUp(void)
{
    test_state = {};
    sm = {};
}

void tearDown(void)
{
}

void test_init_steps_to_wait_for_programmer(void)
{
    TEST_ASSERT_EQUAL(STATE_INIT, sm.state);
    step_state_machine(sm);
    TEST_ASSERT_EQUAL(STATE_WAIT_FOR_PROGRAMMER, sm.state);
}

void test_wait_for_programmer_keeps_waiting_if_data_wasnt_start_byte(void)
{
    const uint8_t NOT_START_BYTE = 0x00;
    sm.state = STATE_WAIT_FOR_PROGRAMMER;
    set_receive_and_checksum_return_value(NOT_START_BYTE, resp_ok);
    step_state_machine(sm);
    TEST_ASSERT_EQUAL(STATE_WAIT_FOR_START_BYTE, sm.state);
    TEST_ASSERT_EQUAL(1, sm.calculated_checksum);
}

void test_wait_for_programmer_wants_command_if_there_was_start_byte(void)
{
    sm.state = STATE_WAIT_FOR_PROGRAMMER;
    set_receive_and_checksum_return_value(START_BYTE, resp_ok);
    step_state_machine(sm);
    TEST_ASSERT_EQUAL(STATE_COMMAND, sm.state);
    TEST_ASSERT_NOT_EQUAL(0, sm.calculated_checksum);
}

void test_wait_for_programmer_times_out(void)
{
    const uint8_t dontcare = 0;
    sm.state = STATE_WAIT_FOR_PROGRAMMER;
    set_receive_and_checksum_return_value(dontcare, resp_timeout);
    step_state_machine(sm);
    TEST_ASSERT_EQUAL(STATE_EXIT, sm.state);
}

void test_command_steps_to_data(void)
{
    sm.state = STATE_COMMAND;
    set_receive_and_checksum_return_value(COMMAND_WRITE_PAGE, resp_ok);
    step_state_machine(sm);
    TEST_ASSERT_EQUAL(STATE_DATA, sm.state);
    TEST_ASSERT_NOT_EQUAL(0, sm.calculated_checksum);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_init_steps_to_wait_for_programmer);
    RUN_TEST(test_wait_for_programmer_keeps_waiting_if_data_wasnt_start_byte);
    RUN_TEST(test_wait_for_programmer_wants_command_if_there_was_start_byte);
    RUN_TEST(test_wait_for_programmer_times_out);
    RUN_TEST(test_command_steps_to_data);

    UNITY_END();
}