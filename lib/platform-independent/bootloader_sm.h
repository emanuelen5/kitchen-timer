#ifndef __BOOTLOADER_STATE_MACHINE_H__
#define __BOOTLOADER_STATE_MACHINE_H__

#include "stdint.h"
#include "avr/boot.h"

static const uint8_t START_BYTE = 0x03;

typedef enum
{
    COMMAND_WRITE_PAGE,
    COMMAND_READ_SIGNATURE,
    COMMAND_BOOT,
} command_t;

enum
{
    resp_ok = 0x00,
    resp_timeout = 0x05,
    resp_nak = 0xaa,
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
        struct
        {
            uint8_t status;
            uint8_t data[3];
            uint16_t checksum;
        } response;
        uint8_t bytes[SPM_PAGESIZE + 2 + 2];
    } data;
} packet_t;

typedef enum
{
    STATE_INIT = 0,
    STATE_WAIT_FOR_PROGRAMMER,
    STATE_WAIT_FOR_START_BYTE,
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

void step_state_machine(state_machine_t &sm);

#endif // __BOOTLOADER_STATE_MACHINE_H__