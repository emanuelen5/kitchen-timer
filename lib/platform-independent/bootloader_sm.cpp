#include "bootloader_sm.h"
#include "string.h"
#include "util/crc16.h"

void set_counter(uint8_t);
void increment_counter(void);
int UART_receive(uint8_t *data);
void write_page(const uint8_t page_offset, const uint8_t *program_buffer);
void read_signature(uint8_t signature[3]);
uint16_t send_and_checksum(uint8_t byte, uint16_t crc);
void UART_send(uint8_t data);

void send_response(response_t &packet)
{
    uint16_t crc16 = 0xffff;
    crc16 = send_and_checksum(START_BYTE, crc16);
    crc16 = send_and_checksum(packet.generic.status, crc16);
    crc16 = send_and_checksum(4, crc16);
    crc16 = send_and_checksum(packet.generic.data[0], crc16);
    crc16 = send_and_checksum(packet.generic.data[1], crc16);
    crc16 = send_and_checksum(packet.generic.data[2], crc16);
    crc16 = send_and_checksum(packet.generic.data[3], crc16);
    UART_send(crc16 >> 8);
    UART_send(crc16 & 0xff);
}

void reset_state_machine(state_machine_t &sm)
{
    sm.calculated_checksum = 0;
}

void step_state_machine(state_machine_t &sm)
{
    uint8_t received_byte;

    switch (sm.state)
    {
    case STATE_INIT:
        reset_state_machine(sm);
        sm.state = STATE_WAIT_FOR_PROGRAMMER;
        break;
    case STATE_WAIT_FOR_PROGRAMMER:
        set_counter(sm.state);
        if (UART_receive(&received_byte) == resp_timeout)
        {
            sm.state = STATE_EXIT;
            return;
        }

        sm.state = (received_byte == START_BYTE) ? STATE_COMMAND : STATE_WAIT_FOR_START_BYTE;
        break;

    case STATE_WAIT_FOR_START_BYTE:
        set_counter(sm.state);
        if (UART_receive(&received_byte) != resp_ok)
        {
            reset_state_machine(sm);
            return;
        }

        sm.state = STATE_COMMAND;
        break;

    case STATE_COMMAND:
        set_counter(sm.state);
        if (UART_receive(&received_byte) != resp_ok)
            return;

        sm.packet.command = (command_t)received_byte;
        sm.state = STATE_LENGTH;
        break;

    case STATE_LENGTH:
        set_counter(sm.state);
        if (UART_receive(&received_byte) != resp_ok)
            return;

        sm.data_index = 0;
        sm.packet.data_length = received_byte + 2;
        sm.state = STATE_DATA;
        break;

    case STATE_DATA:
        set_counter(sm.state);
        if (UART_receive(&received_byte) != resp_ok)
            return;

        increment_counter();
        sm.packet.data.bytes[sm.data_index++] = received_byte;
        if (sm.data_index >= sm.packet.data_length)
        {
            sm.state = STATE_CHECK_CHECKSUM;
        }
        break;

    case STATE_CHECK_CHECKSUM:
        set_counter(sm.state);
        sm.calculated_checksum = 0xffff;
        sm.calculated_checksum = _crc16_update(sm.calculated_checksum, START_BYTE);
        sm.calculated_checksum = _crc16_update(sm.calculated_checksum, sm.packet.command);
        sm.calculated_checksum = _crc16_update(sm.calculated_checksum, sm.packet.data_length);
        sm.incoming_checksum = 0;
        for (uint8_t i = 0; i < sm.packet.data_length; i++)
        {
            sm.calculated_checksum = _crc16_update(sm.calculated_checksum, sm.packet.data.bytes[i]);
            // Save the response checksum
            if (i == sm.packet.data_length - 2)
            {
                sm.incoming_checksum = sm.calculated_checksum;
            }
        }
        if (sm.calculated_checksum != 0)
        {
            sm.state = STATE_WRONG_CHECKSUM;
            return;
        }
        sm.state = STATE_RUN_COMMAND;
        break;

    case STATE_WRONG_CHECKSUM:
        set_counter(sm.state);
        sm.response.generic.status = resp_nak;
        sm.response.generic.data[0] = sm.incoming_checksum >> 8;
        sm.response.generic.data[1] = sm.incoming_checksum & 0xff;
        sm.response.generic.data[2] = sm.calculated_checksum >> 8;
        sm.response.generic.data[3] = sm.calculated_checksum & 0xff;
        sm.state = STATE_RETURN_STATUS;
        break;

    case STATE_RUN_COMMAND:
        set_counter(sm.state);
        memset(&sm.response.bytes, 0, sizeof(sm.response.bytes));
        switch (sm.packet.command)
        {
        case COMMAND_WRITE_PAGE:
            write_page(sm.packet.data.write.page_offset, sm.packet.data.write.data);
            sm.response.generic.status = resp_ack;
            break;
        case COMMAND_READ_SIGNATURE:
            sm.response.generic.status = resp_ack;
            read_signature(&sm.response.generic.data[0]);
            break;
        case COMMAND_BOOT:
            sm.response.generic.status = resp_ack;
            break;
        default:
            sm.response.generic.status = resp_data_unknown_command;
        }

        sm.state = STATE_RETURN_STATUS;
        break;

    case STATE_RETURN_STATUS:
        set_counter(sm.state);
        send_response(sm.response);

        sm.state = sm.packet.command == COMMAND_BOOT ? STATE_EXIT : STATE_WAIT_FOR_START_BYTE;
        break;

    case STATE_EXIT:
        break;
    }
}
