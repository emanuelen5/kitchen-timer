#ifndef LIB_MAX72XX_H
#define LIB_MAX72XX_H

#include "stdint.h"

typedef enum
{
    Max72XX_Command_Nop = 0x00,
    Max72XX_Digit0 = 0x01,
    Max72XX_Digit1 = 0x02,
    Max72XX_Digit2 = 0x03,
    Max72XX_Digit3 = 0x04,
    Max72XX_Digit4 = 0x05,
    Max72XX_Digit5 = 0x06,
    Max72XX_Digit6 = 0x07,
    Max72XX_Digit7 = 0x08,
    Max72XX_Decode_Mode = 0x09,
    Max72XX_Intensity = 0x0A,
    Max72XX_Scan_Limit = 0x0B,
    Max72XX_Shutdown = 0x0C,
    Max72XX_Display_Test = 0x0F,
} max72xx_reg_t;

typedef struct
{
    max72xx_reg_t reg;
    uint8_t data;
} max72xx_cmd_t;

void init_max72xx(void);
void max72xx_write_byte(uint8_t reg, uint8_t data);
void max72xx_clear(void);

#endif // LIB_MAX72XX_H
