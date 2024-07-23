#include <avr/io.h>
#include <util/delay.h>
#include "max72xx.h"
#include "util.h"
#include "SPI.h"

#define CS_PIN PB2

void max72xx_write_byte(uint8_t reg, uint8_t data);

void init_max72xx(void)
{
    init_SPI(CS_PIN);

    max72xx_write_byte(Max72XX_Shutdown, 0x01);     //normal shutdown
    max72xx_write_byte(Max72XX_Scan_Limit, 0x07);   //8 digits scan limit
    max72xx_write_byte(Max72XX_Decode_Mode, 0x00);  //disable decode mode
    max72xx_write_byte(Max72XX_Intensity, 0x0F);    //brightness
    max72xx_write_byte(Max72XX_Display_Test, 0x00); //disable_display_test

}

static void inline deactivate_cs(void)
{
    PORTD |= bit(CS_PIN);
}

static void inline activate_cs(void)
{
    PORTD &= ~bit(CS_PIN);
}

void max72xx_write_byte(uint8_t reg, uint8_t data)
{
    activate_cs();
    SPI_transmit_byte((max72xx_reg_t)reg);
    SPI_transmit_byte(data);
    deactivate_cs();
}

void max72xx_clear(void)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        max72xx_write_byte(Max72XX_Digit0 + i, 0x00);
    }
}
