#ifndef __BOOTLOADER_STATE_MACHINE_H__
#define __BOOTLOADER_STATE_MACHINE_H__

#include "stdint.h"

// #include "avr/boot.h"
#define SPM_PAGESIZE 64

static const uint8_t START_BYTE = 0x03;

typedef enum
{
    COMMAND_READ_SIGNATURE,
    COMMAND_VERSION,
    COMMAND_WRITE_PAGE,
    COMMAND_BOOT,
} command_t;

enum
{
    resp_ack = 0x10,
    resp_nak = 0x11,
    resp_data_unknown_command = 0x12,
};

typedef struct
{
    command_t command;
    uint8_t data_length;
    union
    {
        struct
        {
            uint16_t page_offset;
            uint16_t checksum;
        } read;
        struct
        {
            uint16_t page_offset;
            uint8_t data[SPM_PAGESIZE];
            uint16_t checksum;
        } write;
        uint8_t bytes[SPM_PAGESIZE + 2 + 2];
    } data;
} rx_packet_t;

typedef struct
{
    struct
    {
        uint8_t status;
        uint8_t data[4];
    } generic;
    uint8_t bytes[5];
} response_t;

typedef enum
{
    STATE_INIT = 0,
    STATE_WAIT_FOR_PROGRAMMER,
    STATE_WAIT_FOR_START_BYTE,
    STATE_COMMAND,
    STATE_LENGTH,
    STATE_DATA,
    STATE_CHECK_CHECKSUM,
    STATE_WRONG_CHECKSUM,
    STATE_RUN_COMMAND,
    STATE_RETURN_STATUS,
    STATE_EXIT,
} state_t;

typedef struct
{
    state_t state;

    uint16_t calculated_checksum;
    uint16_t incoming_checksum;

    rx_packet_t packet = {};
    response_t response = {};
    uint8_t data_index = 0;
} state_machine_t;

void step_state_machine(state_machine_t &sm);

#endif // __BOOTLOADER_STATE_MACHINE_H__