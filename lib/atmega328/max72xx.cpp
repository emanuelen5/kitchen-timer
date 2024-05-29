#include "max72xx.h"
#include "util.h"
#include <avr/io.h>
#include <util/delay.h>

#define PD_CLK PD5
#define PD_CS PD6
#define PD_DATA PD7

static void inline deactivate_cs(void);
static void inline activate_cs(void);
static void assert_clock(void);
static void inline deassert_clock(void);

void init_max72xx(void)
{
    PORTD &= ~(bit(PD_CLK) | bit(PD_DATA));
    deactivate_cs();
    DDRD |= bit(PD_CLK) | bit(PD_CS) | bit(PD_DATA);
}

static void send_byte(uint8_t data)
{
    for (int8_t i = 7; i >= 0; i--)
    {
        bool bit_at_offset_i_is_set = bit(7) & data;
        uint8_t b = PORTD & ~bit(PD_DATA);
        PORTD = b | (bit_at_offset_i_is_set ? bit(PD_DATA) : 0);
        _delay_us(1);

        assert_clock();
        _delay_us(1);

        deassert_clock();
        data <<= 1;
    }
}

void max72xx_send_commands(max72xx_cmd_t *cmds, uint8_t length)
{
    activate_cs();

    for (uint8_t i = 0; i < length; i++)
    {
        send_byte((uint8_t)cmds[i].reg);
        send_byte(cmds[i].data);
    }

    deactivate_cs();
}

static void inline deactivate_cs(void)
{
    PORTD |= bit(PD_CS);
}

static void inline activate_cs(void)
{
    PORTD &= ~bit(PD_CS);
}

static void assert_clock(void)
{
    PORTD |= bit(PD_CLK);
}

static void inline deassert_clock(void)
{
    PORTD &= ~bit(PD_CLK);
}