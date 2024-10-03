#include "max72xx.h"
#include <avr/io.h>
#include <util/delay.h>
#include "util.h"
#include "SPI.h"

#define CS_PIN PB2

void max72xx_write_byte(uint8_t device, uint8_t reg, uint8_t data);

void init_max72xx(void)
{
    init_SPI(CS_PIN);
    for (uint8_t device = 0; device < MAX72XX_NUM_DEVICES; device++)
    {
        max72xx_write_byte(device, Max72XX_Shutdown, 0x01);     //normal shutdown
        max72xx_write_byte(device, Max72XX_Scan_Limit, 0x07);   //8 digits scan limit
        max72xx_write_byte(device, Max72XX_Decode_Mode, 0x00);  //disable decode mode
        max72xx_write_byte(device, Max72XX_Intensity, 0x0F);    //brightness
        max72xx_write_byte(device, Max72XX_Display_Test, 0x00); //disable_display_test
    }
    max72xx_clear();
}

static void inline deactivate_cs(void)
{
    PORTB |= bit(CS_PIN);
}

static void inline activate_cs(void)
{
    PORTB &= ~bit(CS_PIN);
}

void max72xx_write_byte(uint8_t device, uint8_t reg, uint8_t data)
{
    activate_cs();
    for( uint8_t d = 0; d < MAX72XX_NUM_DEVICES; d++)
    {
        if ( d == device)
        {
            SPI_transmit_byte(reg);
            SPI_transmit_byte(data);
        }
        else
        {
            SPI_transmit_byte(Max72XX_Command_Nop);
            SPI_transmit_byte(0x00);
        }
    }
    deactivate_cs();
}

void max72xx_clear(void)
{
    for(uint8_t device = 0; device < MAX72XX_NUM_DEVICES; device++)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            max72xx_write_byte(device, Max72XX_Digit0 + i, 0x00);
        }
    }
}

void max72xx_set_intensity(uint8_t intensity)
{
    if(intensity > 0x0F) intensity = 0x0F;

    for (uint8_t device = 0; device < MAX72XX_NUM_DEVICES; device++)
    {
        max72xx_write_byte(device, Max72XX_Intensity, intensity);
    }
}

void max72xx_shutdown(bool shutdown_mode)
{
    for(uint8_t device = 0; device < MAX72XX_NUM_DEVICES; device++)
    {
        max72xx_write_byte(device, Max72XX_Shutdown, shutdown_mode ? 0x00 : 0x01);
    }
}

void max72xx_display_test(bool test_mode)
{
    for(uint8_t device = 0; device < MAX72XX_NUM_DEVICES; device++)
    {
        max72xx_write_byte(device, Max72XX_Display_Test, test_mode ? 0x01 : 0x00);
    }
}
