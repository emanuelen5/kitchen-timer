#include <unity.h>
#include <stdio.h>
#include <stdarg.h>

#include "state-machine.h"

// Test doubles
uint16_t millis(void)
{
    return 0;
}

void set_counter(uint8_t count)
{
    (void)count;
}

void UART_printf(char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

state_machine_t sm;

void setUp(void)
{
    init_state_machine(&sm);
}

void tearDown(void)
{
}

void test_initialize_as_idle(void)
{
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_initialize_as_idle);

    UNITY_END();
}