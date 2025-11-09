#include <avr/io.h>
#include <avr/interrupt.h>

#include "util.h"
#include "rtc.h"
#include "uint8-queue.h"
#include "UART.h"
#include "millis.h"
#include "render.h"
#include "application.h"
#include "led-counter.h"
#include "avr_button.h"
#include "rotary-encoder.h"
#include "max72xx.h"
#include "render.h"
#include "max72xx_matrix.h"
#include <util/delay.h>

uint8_queue_t eventQueue;
static const uint8_t queue_buffer_size = 8;
uint8_t event_queue_buffer[queue_buffer_size];

application_t app;

void rotation_cb(rotation_dir_t dir, rotation_speed_t speed, bool held_down)
{
    event_t event;
    if (held_down && dir == cw)
        event = CW_PRESSED_ROTATION;
    else if (held_down && dir == ccw)
        event = CCW_PRESSED_ROTATION;
    else if (!held_down && dir == cw && speed == fast)
        event = CW_ROTATION_FAST;
    else if (!held_down && dir == cw)
        event = CW_ROTATION;
    else if (!held_down && dir == ccw && speed == fast)
        event = CCW_ROTATION_FAST;
    else if (!held_down && dir == ccw)
        event = CCW_ROTATION;

    add_to_queue(&eventQueue, event);
}

void on_single_press(void)
{
    add_to_queue(&eventQueue, SINGLE_PRESS);
}

void on_double_press(void)
{
    add_to_queue(&eventQueue, DOUBLE_PRESS);
}

void on_long_press(void)
{
    add_to_queue(&eventQueue, LONG_PRESS);
}

void second_tick_cb(void)
{
    add_to_queue(&eventQueue, SECOND_TICK);
}

void init_hw_adc(void)
{
}

// Voltage is scaled 1024 / 4.3 / 1.1
uint16_t battery_centivolts(void)
{
    ADMUX = bit(REFS1) | bit(REFS0) | bit(MUX2) | bit(MUX1) | bit(MUX0); // Vref=1.1V, ADC7
    ADCSRA = bit(ADEN) | bit(ADPS2) | bit(ADPS1) | bit(ADPS0);           // Enable ADC, prescaler=128
    max72xx_set_intensity(0);                                            // Minimize display load during ADC
    _delay_ms(2);                                                        // Allow voltage to stabilize
    ADCSRA |= bit(ADSC);                                                 // Start conversion
    while (ADCSRA & bit(ADSC))
        ;                                                    // Wait for conversion to complete
    uint16_t adc_value = ADC;                                // Read ADC value
    uint32_t voltage = (uint32_t)adc_value * 11 * 43 / 1024; // Scale to centivolts
    max72xx_set_intensity(app.brightness);                   // Restore brightness
    ADCSRA = 0;
    return (uint16_t)voltage;
}

void draw_voltage(uint16_t decivolts)
{
    const uint8_t x_offset = 0, y_offset = 0;
    matrix_buffer_clear();

    uint8_t digits[3];
    digits[0] = '0' + (decivolts / 100);        // Volts
    digits[1] = '0' + ((decivolts % 100) / 10); // First decimal
    digits[2] = '0' + (decivolts % 10);         // Second decimal

    draw_char('V', x_offset, y_offset, false);
    draw_char(digits[0], x_offset + 7, y_offset, false);
    matrix_set_pixel(x_offset + 14, y_offset + 6, true); // Decimal point

    draw_char(digits[1], x_offset, y_offset + 8, false);
    draw_char(digits[2], x_offset + 7, y_offset + 8, false);

    matrix_update();
}

int main()
{
    AvrButton button(&on_single_press, &on_double_press, &on_long_press);

    init_hw_UART();
    init_hw_timer2_to_1s_interrupt(&second_tick_cb);
    init_hw_millis();
    init_hw_led_counter();
    init_hw_max72xx();
    init_hw_adc();
    init_queue(&eventQueue, event_queue_buffer, queue_buffer_size);
    init_hw_rotary_encoder(rotation_cb, button);
    init_application(&app);
    max72xx_set_intensity(app.brightness);
    sei();

    init_max72xx();

    while (true)
    {
        for (uint8_t count = UART_received_char_count(); count != 0; count--)
        {
            service_receive_UART();
        }
        button.service();
        dequeue_return_t event;
        while ((event = dequeue(&eventQueue)).is_valid)
        {
            application_handle_event(&app, (event_t)event.value);
        }
        service_application(&app);
        draw_voltage(battery_centivolts());
    }

    return 0;
}
