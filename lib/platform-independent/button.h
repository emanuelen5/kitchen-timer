#ifndef LIB_BUTTON_H
#define LIB_BUTTON_H

#include "stdint.h"

typedef enum
{
    ccw,
    cw,
    none,
} rotation_dir_t;

typedef enum
{
    slow,
    fast,
} rotation_speed_t;

typedef void (*rotation_cb_t)(rotation_dir_t dir, rotation_speed_t speed, bool held_down);

class Button
{
public:
    Button(void (*single_press_handler)(),
           void (*double_press_handler)(),
           void (*long_press_handler)(),
           rotation_cb_t rotation_handler);

    void press();
    void release();
    void rotate(rotation_dir_t dir, rotation_speed_t speed);
    virtual void service();

    inline bool get_is_pressed()
    {
        return is_pressed;
    }

    static constexpr uint16_t long_press_threshold_ms = 1000;
    static constexpr uint16_t double_press_timeout_ms = 300;

private:
    uint16_t last_press_time;
    uint8_t press_count;
    bool is_pressed;

    // Callbacks provided by user
    void (*on_single_press)();
    void (*on_double_press)();
    void (*on_long_press)();
    rotation_cb_t on_rotation;

    void switch_to_rotation();
    void invoke_single_press(void);
    void invoke_double_press(void);
    void invoke_long_press(void);
};

#endif // LIB_BUTTON_H
