#ifndef LIB_MAX72XX_H
#define LIB_MAX72XX_H

#include "stdint.h"

#define MAX72XX_NUM_DEVICES 4

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

void init_hw_max72xx(void);
void max72xx_send_commands(max72xx_cmd_t *cmds, uint8_t length);
void max72xx_send_commands_to_all(max72xx_reg_t reg, uint8_t data);
void max72xx_set_intensity(uint8_t intensity);
void max72xx_shutdown(bool shutdown);
void max72xx_display_test(bool test_mode);
uint8_t get_intensity(void);

#endif // LIB_MAX72XX_H
