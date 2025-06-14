#include "bootloader_sm.h"
#include "string.h"

void set_counter(uint8_t);
void increment_counter(void);
int UART_receive_with_timeout(uint8_t *data);
uint8_t UART_receive(void);
void write_page(const uint16_t page_offset, const uint8_t *program_buffer);
void read_page(const uint16_t page_offset, uint8_t *program_buffer);
void read_signature(uint8_t signature[3]);
uint16_t send_and_checksum(uint8_t byte, uint16_t crc);
uint16_t checksum(uint16_t crc, uint8_t byte);
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
    UART_send(crc16 & 0xff);
    UART_send(crc16 >> 8);
}

void send_page_response(uint16_t page_offset, uint8_t *data)
{
    read_page(page_offset, data);
    uint16_t crc16 = 0xffff;
    crc16 = send_and_checksum(START_BYTE, crc16);
    crc16 = send_and_checksum(resp_ack, crc16);
    crc16 = send_and_checksum(2 + SPM_PAGESIZE, crc16);
    crc16 = send_and_checksum(page_offset & 0xff, crc16);
    crc16 = send_and_checksum(page_offset >> 8, crc16);
    for (uint8_t i = 0; i < SPM_PAGESIZE; i++)
    {
        crc16 = send_and_checksum(data[i], crc16);
    }
    UART_send(crc16 & 0xff);
    UART_send(crc16 >> 8);
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
        if (UART_receive_with_timeout(&received_byte))
        {
            sm.state = STATE_EXIT;
            return;
        }

        sm.state = (received_byte == START_BYTE) ? STATE_COMMAND : STATE_WAIT_FOR_START_BYTE;
        break;

    case STATE_WAIT_FOR_START_BYTE:
        set_counter(sm.state);
        received_byte = UART_receive();
        sm.state = STATE_COMMAND;
        break;

    case STATE_COMMAND:
        set_counter(sm.state);
        received_byte = UART_receive();
        sm.packet.command = (command_t)received_byte;
        sm.state = STATE_LENGTH;
        break;

    case STATE_LENGTH:
        set_counter(sm.state);
        received_byte = UART_receive();
        sm.data_index = 0;
        sm.packet.data_length = received_byte;
        sm.state = STATE_DATA;
        break;

    case STATE_DATA:
        set_counter(sm.state);
        received_byte = UART_receive();
        increment_counter();
        sm.packet.data.bytes[sm.data_index++] = received_byte;
        if (sm.data_index >= sm.packet.data_length + 2)
        {
            sm.state = STATE_CHECK_CHECKSUM;
        }
        break;

    case STATE_CHECK_CHECKSUM:
        set_counter(sm.state);
        sm.calculated_checksum = 0xffff;
        sm.calculated_checksum = checksum(sm.calculated_checksum, START_BYTE);
        sm.calculated_checksum = checksum(sm.calculated_checksum, sm.packet.command);
        sm.calculated_checksum = checksum(sm.calculated_checksum, sm.packet.data_length);
        for (uint8_t i = 0; i < sm.packet.data_length + 2; i++)
        {
            sm.calculated_checksum = checksum(sm.calculated_checksum, sm.packet.data.bytes[i]);
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
        sm.response.generic.data[0] = 0;
        sm.response.generic.data[1] = 0;
        sm.response.generic.data[2] = sm.calculated_checksum & 0xff;
        sm.response.generic.data[3] = sm.calculated_checksum >> 8;
        sm.state = STATE_RETURN_STATUS;
        break;

    case STATE_RUN_COMMAND:
        set_counter(sm.state);
        memset(sm.response.bytes, 0, sizeof(sm.response.bytes));
        switch (sm.packet.command)
        {
        case COMMAND_WRITE_PAGE:
            write_page(sm.packet.data.write.page_offset, sm.packet.data.write.data);
            sm.response.generic.status = resp_ack;
            sm.response.generic.data[0] = sm.packet.data.write.page_offset & 0xff;
            sm.response.generic.data[1] = sm.packet.data.write.page_offset >> 8;
            sm.response.generic.data[2] = 0;
            sm.response.generic.data[3] = 0;
            break;
        case COMMAND_READ_SIGNATURE:
            sm.response.generic.status = resp_ack;
            read_signature(&sm.response.generic.data[0]);
            break;
        case COMMAND_READ_PAGE:
            sm.state = STATE_READ_PAGE;
            return;
            break;
        case COMMAND_BOOT:
            sm.response.generic.status = resp_ack;
            break;
        default:
            sm.response.generic.status = resp_data_unknown_command;
        }

        sm.state = STATE_RETURN_STATUS;
        break;

    case STATE_READ_PAGE:
        set_counter(sm.state);
        send_page_response(sm.packet.data.read.page_offset, sm.packet.data.write.data);
        sm.state = STATE_WAIT_FOR_START_BYTE;
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
