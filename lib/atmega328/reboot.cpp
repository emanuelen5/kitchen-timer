#include "reboot.h"
#include <avr/io.h>
#include "util.h"
#include <avr/wdt.h>
#include <avr/interrupt.h>

void init_watchdog(void)
{
    MCUSR = bit(WDRF);
    wdt_disable();
}

void reboot_system(void)
{
    cli();
    wdt_enable(WDTO_15MS);
    while(true);
}
