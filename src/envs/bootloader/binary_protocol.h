#ifndef _BINARY_PROTOCOL_H_
#define _BINARY_PROTOCOL_H_

#include "stdint.h"
#include "avr/boot.h"

typedef enum
{
    COMMAND_WRITE_PAGE,
    COMMAND_READ_SIGNATURE,
    COMMAND_BOOT,
} command_t;

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

#endif // _BINARY_PROTOCOL_H_