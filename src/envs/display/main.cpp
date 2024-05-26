#include "display.h"
#include <util/delay.h>

static void set_display_into_test_mode(bool is_test)
{
    uint8_t data = is_test ? 0x01 : 0x00;
    max72xx_cmd_t cmds[4] = {
        {Max72XX_Display_Test, data},
        {Max72XX_Display_Test, data},
        {Max72XX_Display_Test, data},
        {Max72XX_Display_Test, data},
    };
    set_display_registers(cmds);
}

int main(void)
{
    init_display();

    for (;;)
    {
        set_display_into_test_mode(false);
        _delay_ms(1000);
        set_display_into_test_mode(true);
        _delay_ms(1000);
    }
    return 0;
}