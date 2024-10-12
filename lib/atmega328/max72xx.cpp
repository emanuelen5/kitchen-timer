#include "SPI.h"
#include "max72xx.h"
#include <avr/io.h>
#include <util/delay.h>
#include "util.h"

#define CS_PIN PB2

SPI_context_t spi_cmds;

void max72xx_send_commands_to_all(max72xx_reg_t reg, uint8_t data);

void init_max72xx(void)
{
    init_SPI(CS_PIN);

    max72xx_send_commands_to_all(Max72XX_Shutdown, 0x01);        // normal operation (exit shutdown mode)
    max72xx_send_commands_to_all(Max72XX_Scan_Limit, 0x07);      // 8 digits scan limit
    max72xx_send_commands_to_all(Max72XX_Decode_Mode, 0x00);     // disable decode mode
    max72xx_send_commands_to_all(Max72XX_Intensity, 0x0F);       // set brightness
    max72xx_send_commands_to_all(Max72XX_Display_Test, 0x00);    // disable display test
}

void deactivate_cs(void)
{
    PORTB |= bit(CS_PIN);
}

void activate_cs(void)
{
    PORTB &= ~bit(CS_PIN);
}

void max72xx_send_commands(max72xx_cmd_t *cmds, uint8_t length)
{
    spi_cmds = {cmds, length, 0}; //struct that is passed to the ISR(SPI_STC_vect)

    activate_cs();
    SPI_transmit_byte(spi_cmds.cmds[spi_cmds.index/2].reg);
}

void max72xx_send_commands_to_all(max72xx_reg_t reg, uint8_t data)
{
    max72xx_cmd_t cmds[MAX72XX_NUM_DEVICES];
    for (int i=0; i < MAX72XX_NUM_DEVICES; i++)
    {
        cmds[i].reg = reg;
        cmds[i].data = data;
    }
    max72xx_send_commands(cmds, MAX72XX_NUM_DEVICES);
}

void max72xx_set_intensity(uint8_t intensity)
{
    if(intensity > 0x0F) intensity = 0x0F;
    max72xx_send_commands_to_all(Max72XX_Intensity, intensity);
}

void max72xx_shutdown(bool shutdown_mode)
{
    max72xx_send_commands_to_all(Max72XX_Shutdown, shutdown_mode ? 0x00 : 0x01);
}

void max72xx_display_test(bool test_mode)
{
    max72xx_send_commands_to_all(Max72XX_Display_Test, test_mode ? 0x00 : 0x01);
}
