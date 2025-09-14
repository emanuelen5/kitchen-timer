#ifndef LIB_BUTTON_H
#define LIB_BUTTON_H

#include "stdint.h"

class Button
{
public:
    Button(void (*single_press_handler)(),
           void (*double_press_handler)(),
           void (*long_press_handler)());

    void press();
    void release();
    virtual void service();

    inline bool get_is_pressed()
    {
        return is_pressed;
    }

    void switch_to_rotation();

    static constexpr uint16_t long_press_threshold_ms = 2000;
    static constexpr uint16_t double_press_timeout_ms = 500;
    static constexpr uint16_t press_to_rotation_timeout_ms = 100;

private:
    uint16_t last_press_time;
    uint8_t press_count;
    bool is_pressed;

    // Callbacks provided by user
    void (*on_single_press)();
    void (*on_double_press)();
    void (*on_long_press)();

    void invoke_single_press(void);
    void invoke_double_press(void);
    void invoke_long_press(void);
};

#endif // LIB_BUTTON_H
