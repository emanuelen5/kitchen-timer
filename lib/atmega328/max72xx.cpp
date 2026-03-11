#include "max72xx.h"
#include <avr/io.h>
#include "util.h"
#include "SPI.h"
#include "config.h"
#include <util/delay.h>
#include "boost_converter.h"

void max72xx_send_commands(max72xx_cmd_t *cmds, uint8_t length);
void max72xx_send_commands_to_all(max72xx_reg_t reg, uint8_t data);

void init_hw_max72xx(void)
{
    init_hw_SPI(MAX72XX_NUM_DEVICES * 2);
}

static void clear_display()
{
    for (uint8_t digit = Max72XX_Digit0; digit <= Max72XX_Digit7; digit++)
    {
        max72xx_send_commands_to_all((max72xx_reg_t)digit, 0x00);
    }
}

void init_max72xx(void)
{
    clear_display();
    max72xx_send_commands_to_all(Max72XX_Display_Test, 0x00); // disable display test
    max72xx_send_commands_to_all(Max72XX_Intensity, 0);
    max72xx_send_commands_to_all(Max72XX_Scan_Limit, 0x07);   // 8 digits scan limit
    max72xx_send_commands_to_all(Max72XX_Decode_Mode, 0x00);  // disable decode mode
    max72xx_send_commands_to_all(Max72XX_Shutdown, 0x01); // normal operation (exit shutdown mode)
    for (int i = 0; i <= max72xx_max_brightness; i++)
    {
        _delay_ms(10);
        max72xx_reg_t digit = (max72xx_reg_t)(Max72XX_Digit0 + i);
        if (digit <= Max72XX_Digit7)
            max72xx_send_commands_to_all(digit, 0xff);
    }
    for (int i = 0; i <= max72xx_max_brightness; i++)
    {
        _delay_ms(10);
        max72xx_send_commands_to_all(Max72XX_Intensity, i);
    }
}

void max72xx_send_commands(max72xx_cmd_t *cmds, uint8_t length) // adds the cmds 8 bytes array to SPI_queue and starts transmission
{
    for (uint8_t device = 0; device < length; device++)
    {
        add_to_SPI_queue(cmds[device].reg);
        add_to_SPI_queue(cmds[device].data);
    }

    start_SPI_transfer();
}

void max72xx_send_commands_to_all(max72xx_reg_t reg, uint8_t data) //Creates a "cmds" array of 8 bytes and adds to SPI_queue
{
    max72xx_cmd_t cmds[MAX72XX_NUM_DEVICES];
    for (int i = 0; i < MAX72XX_NUM_DEVICES; i++)
    {
        cmds[i].reg = reg;
        cmds[i].data = data;
    }
    max72xx_send_commands(cmds, MAX72XX_NUM_DEVICES);
}

void max72xx_set_intensity(uint8_t intensity)
{
    if (intensity > max72xx_max_brightness)
        intensity = max72xx_max_brightness;
    max72xx_send_commands_to_all(Max72XX_Intensity, intensity);
}

void max72xx_display_on()
{
    bc_power_safe_off(); // Enable boost converter first
    _delay_ms(5); // Allow boost converter to stabilize
    max72xx_send_commands_to_all(Max72XX_Shutdown, 0x01); // normal operation (exit shutdown mode)
}

void max72xx_display_off()
{
    max72xx_send_commands_to_all(Max72XX_Shutdown, 0x00); // Shutdown display first
    _delay_ms(1); // Allow display to turn off
    bc_power_safe_on(); // Then disable boost converter
}

void max72xx_display_test(bool test_mode)
{
    max72xx_send_commands_to_all(Max72XX_Display_Test, test_mode ? 0x00 : 0x01);
}
