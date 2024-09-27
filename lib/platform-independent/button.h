#ifndef LIB_BUTTON_H
#define LIB_BUTTON_H

#include "stdint.h"

class Button
{
public:
    Button();
    void press();
    void release();
    void service();

    static constexpr uint16_t long_press_threshold_ms = 2000;
    static constexpr uint16_t double_press_timeout_ms = 500;

private:
    uint16_t last_press_time;
    uint8_t press_count;
    bool is_pressed;

    // Must be implemented by user
    void on_single_press();
    void on_double_press();
    void on_long_press();
};

#endif // LIB_BUTTON_H
