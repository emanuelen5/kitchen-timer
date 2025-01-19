#include "bootloader_sm.h"

void set_counter(uint8_t);
void increment_counter(void);
int receive_and_checksum(uint8_t *byte, uint16_t *crc);
void write_page(const uint8_t page_offset, const uint8_t *program_buffer);
void read_signature(uint8_t signature[3]);
void send_response(packet_t &packet);

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
        if (receive_and_checksum(&received_byte, &sm.calculated_checksum) == resp_timeout)
        {
            sm.state = STATE_EXIT;
            return;
        }

        sm.state = (received_byte == START_BYTE) ? STATE_COMMAND : STATE_WAIT_FOR_START_BYTE;
        break;

    case STATE_WAIT_FOR_START_BYTE:
        set_counter(sm.state);
        if (receive_and_checksum(&received_byte, &sm.calculated_checksum) != resp_ok)
        {
            reset_state_machine(sm);
            return;
        }

        sm.state = STATE_COMMAND;
        break;

    case STATE_COMMAND:
        set_counter(sm.state);
        if (receive_and_checksum(&received_byte, &sm.calculated_checksum) != resp_ok)
            return;

        sm.packet.command = (command_t)received_byte;
        if (sm.packet.command == COMMAND_WRITE_PAGE)
        {
            sm.state = STATE_DATA;
        }
        break;

    case STATE_LENGTH:
        set_counter(sm.state);
        if (receive_and_checksum(&received_byte, &sm.calculated_checksum) != resp_ok)
            return;

        sm.packet.data_length = received_byte + 2;
        sm.state = STATE_DATA;
        break;

    case STATE_DATA:
        if (receive_and_checksum(&received_byte, &sm.calculated_checksum) != resp_ok)
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
        if (sm.calculated_checksum != 0)
        {
            sm.packet.data.response.status = resp_nak;
            sm.state = STATE_RETURN_STATUS;
            return;
        }
        sm.state = STATE_RUN_COMMAND;
        break;

    case STATE_RUN_COMMAND:
        switch (sm.packet.command)
        {
        case COMMAND_WRITE_PAGE:
            write_page(sm.packet.data.write.page_offset, sm.packet.data.write.data);
            sm.packet.data.response.status = resp_ok;
            break;
        case COMMAND_READ_SIGNATURE:
            sm.packet.data.response.status = resp_ok;
            read_signature(&sm.packet.data.response.data[0]);
            break;
        case COMMAND_BOOT:
            sm.packet.data.response.status = resp_ok;
            break;
        default:
            sm.packet.data.response.data[0] = resp_data_unknown_command;
            sm.packet.data.response.status = resp_nak;
        }

        sm.state = STATE_RETURN_STATUS;
        break;

    case STATE_RETURN_STATUS:
        sm.packet.data_length = 1;
        set_counter(sm.state);
        send_response(sm.packet);

        if (sm.packet.command == COMMAND_BOOT)
        {
            return;
        }

        sm.state = STATE_WAIT_FOR_START_BYTE;
        break;

    case STATE_EXIT:
        break;
    }
}
