#include <avr/io.h>
#include <util/delay.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/crc16.h>
#include "led-counter.h"
#include "millis.h"
#include "binary_protocol.h"

#if F_CPU != 1000000UL
#error The library can only handle a CPU frequency of 1MHz at the moment
#endif

static uint8_t sreg_last_state = 0;

void prepare_self_program(void)
{
    sreg_last_state = SREG;
    cli();
    eeprom_busy_wait();
}

void write_page(const uint8_t page_offset, const uint8_t *program_buffer)
{
    prepare_self_program();

    const uint16_t page_address = page_offset * SPM_PAGESIZE;
    boot_page_erase(page_address);
    boot_spm_busy_wait();

    const uint8_t word_size = 2;
    for (uint16_t byte_offset = 0; byte_offset < SPM_PAGESIZE; byte_offset += word_size)
    {
        uint16_t word = 0;
        word = *program_buffer++;
        word |= ((uint16_t)(*program_buffer++)) << 8;
        boot_page_fill(page_address + byte_offset, word);
    }

    boot_page_write(page_address);
    boot_spm_busy_wait();

    SREG = sreg_last_state;
}

void read_signature(uint8_t signature[3])
{
    signature[0] = boot_signature_byte_get(0x00);
    signature[1] = boot_signature_byte_get(0x00);
    signature[2] = boot_signature_byte_get(0x00);
}

void finalize_self_program(void)
{
    // Re-enable RWW-section. We need this to be able to jump back
    // to the application after bootloading.
    boot_rww_enable();
    SREG = sreg_last_state;
}

static inline void
jump_to_start_of_program_and_exit_bootloader(void)
{
    asm("jmp 0");
}

enum
{
    ok = 0x00,
    timeout = 0x05,
    nak = 0xaa,
};

static void UART_send(uint8_t data)
{
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    UDR0 = data;
}

static int UART_receive(uint8_t *data)
{
    uint16_t start_time = millis();
    const uint16_t timeout_duration = 2000;
    while (true)
    {
        if (UCSR0A & (1 << RXC0))
        {
            *data = UDR0;
            return 0; // Successful reception
        }

        if ((uint16_t)(millis() - start_time) >= timeout_duration)
            return timeout;
    }
}

enum
{
    NUL_BYTE = 0x00,
    START_BYTE = 0x03,
};

static int receive_and_checksum(uint8_t *byte, uint16_t *crc16)
{
    int status = UART_receive(byte);
    if (status == ok)
        *crc16 = _crc16_update(*crc16, *byte);
    return status;
}

uint16_t send_and_checksum(uint8_t byte, uint16_t crc16)
{
    UART_send(byte);
    return _crc16_update(crc16, byte);
}

void send_response(packet_t &packet)
{
    uint16_t crc16 = 0;
    crc16 = send_and_checksum(START_BYTE, crc16);
    crc16 = send_and_checksum(packet.command, crc16);
    crc16 = send_and_checksum(packet.data_length, crc16);
    const uint8_t packet_length = 4;
    for (uint8_t i = 0; i < packet_length; i++)
    {
        crc16 = send_and_checksum(packet.data.bytes[i], crc16);
    }
    UART_send(packet.data.response.checksum >> 8);
    UART_send(packet.data.response.checksum & 0xff);
}

typedef enum
{
    STATE_WAIT_FOR_PROGRAMMER,
    STATE_WAIT_FOR_START_BYTE,
    STATE_START,
    STATE_COMMAND,
    STATE_LENGTH,
    STATE_DATA,
    STATE_CHECK_CHECKSUM,
    STATE_RUN_COMMAND,
    STATE_RETURN_STATUS,
    STATE_EXIT,
} state_t;

typedef struct
{
    state_t state;

    uint16_t calculated_checksum;

    packet_t packet = {};
    uint8_t data_index = 0;
} state_machine_t;

void step_state_machine(state_machine_t &sm)
{
    uint8_t received_byte;

    switch (sm.state)
    {
    case STATE_WAIT_FOR_PROGRAMMER:
        set_counter(sm.state);
        if (receive_and_checksum(&received_byte, &sm.calculated_checksum) == timeout)
        {
            sm.state = STATE_EXIT;
            return;
        }

        sm.state = (received_byte == START_BYTE) ? STATE_START : STATE_WAIT_FOR_START_BYTE;
        break;

    case STATE_WAIT_FOR_START_BYTE:
        set_counter(sm.state);
        if (receive_and_checksum(&received_byte, &sm.calculated_checksum) != ok)
            return;

        sm.state = STATE_START;
        break;

    case STATE_START:
        set_counter(sm.state);
        if (received_byte == START_BYTE)
        {
            sm.calculated_checksum = 0;
            sm.calculated_checksum = _crc16_update(sm.calculated_checksum, received_byte);
            sm.state = STATE_COMMAND;
        }
        break;

    case STATE_COMMAND:
        set_counter(sm.state);
        if (UART_receive(&received_byte) != ok)
            return;

        sm.packet.command = (command_t)received_byte;
        sm.calculated_checksum = _crc16_update(sm.calculated_checksum, received_byte);
        if (sm.packet.command == COMMAND_WRITE_PAGE)
        {
            sm.state = STATE_DATA;
        }
        break;

    case STATE_LENGTH:
        set_counter(sm.state);
        if (UART_receive(&received_byte) != ok)
            return;

        sm.packet.data_length = received_byte + 2;
        sm.calculated_checksum = _crc16_update(sm.calculated_checksum, received_byte);
        sm.state = STATE_DATA;
        sm.data_index = 0;
        break;

    case STATE_DATA:
        if (UART_receive(&received_byte) != ok)
            return;

        increment_counter();
        sm.packet.data.bytes[sm.data_index++] = received_byte;
        sm.calculated_checksum = _crc16_update(sm.calculated_checksum, received_byte);
        if (sm.data_index >= sm.packet.data_length)
        {
            sm.state = STATE_CHECK_CHECKSUM;
        }
        break;

    case STATE_CHECK_CHECKSUM:
        set_counter(sm.state);
        if (sm.calculated_checksum != 0)
        {
            sm.packet.data.response.status = nak;
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
            sm.packet.data.response.status = ok;
            break;
        case COMMAND_READ_SIGNATURE:
            sm.packet.data.response.status = ok;
            read_signature(&sm.packet.data.response.data[0]);
            break;
        case COMMAND_BOOT:
            sm.packet.data.response.status = ok;
            break;
        default:
            sm.packet.data.response.status = nak;
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

void run_state_machine(void)
{
    state_machine_t sm = {};
    sm.data_index = 0;
    sm.calculated_checksum = 0;
    sm.state = STATE_WAIT_FOR_PROGRAMMER;

    while (sm.state != STATE_EXIT)
    {
        step_state_machine(sm);
    }
}

void use_bootloader_interrupt_vectors(void)
{
    asm volatile(
        "ldi r16, %[ivce_bit]       \n\t"
        "out %[mcucr], r16          \n\t"
        "ldi r16, %[ivsel_bit]      \n\t"
        "out %[mcucr], r16          \n\t"
        :
        : [ivce_bit] "M"(1 << IVCE),
          [ivsel_bit] "M"(1 << IVSEL),
          [mcucr] "I"(_SFR_IO_ADDR(MCUCR))
        : "r16");
}

void use_application_interrupt_vectors(void)
{
    asm volatile(
        "ldi r16, %[ivce_bit]       \n\t"
        "out %[mcucr], r16          \n\t"
        "ldi r16, %[ivsel_bit]      \n\t"
        "out %[mcucr], r16          \n\t"
        :
        : [ivce_bit] "M"(1 << IVCE),
          [ivsel_bit] "M"(0),
          [mcucr] "I"(_SFR_IO_ADDR(MCUCR))
        : "r16");
}

int main(void)
{
    use_bootloader_interrupt_vectors();
    init_millis();
    init_led_counter();
    set_counter(0x7);
    sei();

    run_state_machine();

    finalize_self_program();
    deinit_millis();
    use_application_interrupt_vectors();
    jump_to_start_of_program_and_exit_bootloader();
}