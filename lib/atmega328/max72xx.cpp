#include <avr/io.h>
#include <util/delay.h>
#include "max72xx.h"
#include "util.h"
#include "SPI.h"

#define CS_PIN PB2

void max72xx_send_commands(max72xx_cmd_t *cmds, uint8_t length);

void init_max72xx(void)
{
    init_SPI(CS_PIN);

    max72xx_cmd_t no_decode_mode = {.reg = Max72XX_Decode_Mode, .data = 0x00};
    max72xx_cmd_t max_brightness = {.reg = Max72XX_Intensity, .data = 0x0F};
    max72xx_cmd_t eight_digits_scan_limit = {.reg = Max72XX_Scan_Limit, .data = 0x07};
    max72xx_cmd_t normal_shutdown = {.reg = Max72XX_Shutdown, .data = 0x01};
    max72xx_cmd_t display_test_off = {.reg = Max72XX_Display_Test, .data = 0x00};

    max72xx_send_commands(&no_decode_mode, 1);
    max72xx_send_commands(&max_brightness, 1);
    max72xx_send_commands(&eight_digits_scan_limit, 1);
    max72xx_send_commands(&normal_shutdown, 1);
    max72xx_send_commands(&display_test_off, 1);
}

static void inline deactivate_cs(void)
{
    PORTD |= bit(CS_PIN);
}

static void inline activate_cs(void)
{
    PORTD &= ~bit(CS_PIN);
}

void max72xx_send_commands(max72xx_cmd_t *cmds, uint8_t length)
{
    activate_cs();
    for (uint8_t i = 0; i < length; i++)
    {
        SPI_transmit_byte((uint8_t)cmds[i].reg);
        SPI_transmit_byte(cmds[i].data);
    }
    deactivate_cs();
}
