#include "serial_commands_cbs.h"
#include <avr/pgmspace.h>
#include "led-counter.h"
#include "UART.h"




void test_led(bool is_on)
{
    is_on ? set_counter(1) : set_counter(0);
}

void version(void)
{
    UART_print_P(PSTR("Kitchen Timer, version 1.0.0\n"));
    UART_print_P(PSTR("Authors: Erasmus Cedernaes, Nicolas Perozzi\n"));
}

void set_active_timer(state_machine_t *active_sm, uint32_t *steps)
{
    reset_timer(&active_sm->timer);
    set_state(active_sm, SET_TIME);
    change_original_time(&active_sm->timer, (int32_t*)steps);
}
