typedef enum
{
    ccw,
    cw,
} rotation_dir_t;

// Interactions
void press(void);
void double_press(void);
void long_press(void);
void rotate(rotation_dir_t dir, bool is_fast = false);
void rotate_pressed(rotation_dir_t dir);