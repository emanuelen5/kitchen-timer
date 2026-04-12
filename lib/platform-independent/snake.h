#ifndef SNAKE_H
#define SNAKE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    int8_t x;
    int8_t y;
} snake_point_t;

typedef enum
{
    SNAKE_READY,
    SNAKE_RUNNING,
    SNAKE_PAUSED,
    SNAKE_GAME_OVER,
    SNAKE_WON,
} snake_status_t;

typedef enum
{
    SNAKE_UP,
    SNAKE_RIGHT,
    SNAKE_DOWN,
    SNAKE_LEFT,
    SNAKE_DIRECTION_COUNT
} snake_direction_t;

constexpr int8_t snake_board_width = 16;
constexpr int8_t snake_board_height = 16;
constexpr uint16_t snake_max_length = snake_board_width * snake_board_height;
constexpr uint8_t snake_body_dirs_bytes = (snake_max_length - 1 + 3) / 4;

typedef struct
{

    snake_point_t head;
    uint8_t body_dirs[snake_body_dirs_bytes];
    snake_point_t food;
    uint16_t last_step_ms;
    uint16_t move_interval_ms;
    uint16_t random_num_generator_state;
    uint16_t length;
    snake_direction_t direction;
    snake_status_t status;
    bool turn_locked_until_step;
} snake_game_t;

uint8_t snake_get_body_dir(const snake_game_t *game, uint16_t idx);
snake_point_t next_head_position(snake_point_t &head, snake_direction_t direction);

/*
 * Iterate over all body positions of the snake, computing each position
 * on the fly from the head and the packed direction array.
 *
 * Usage:
 *   FOR_BODY_POSITION(game, i, pos) {
 *       // pos is the position of body part i
 *   }
 *
 * break and continue work as expected.
 */
#define REVERSE_DIR(dir) ((snake_direction_t)(((dir) + 2) & 0b11))
#define FOR_BODY_POSITION(game_ptr, idx_var, pos_var)                                \
    for (uint16_t idx_var = 0, _fb_once = 1; _fb_once; _fb_once = 0)                 \
        for (snake_point_t pos_var = (game_ptr)->head;                               \
             idx_var < (game_ptr)->length;                                           \
             pos_var = (idx_var < (game_ptr)->length - 1u)                           \
                           ? next_head_position(                                     \
                                 pos_var,                                            \
                                 REVERSE_DIR(snake_get_body_dir(game_ptr, idx_var))) \
                           : pos_var,                                                \
                           idx_var++)

void snake_turn_left(snake_game_t *game);
void snake_turn_right(snake_game_t *game);
void snake_toggle_pause(snake_game_t *game);
void snake_restart(snake_game_t *game, uint16_t seed);
void service_snake_game(snake_game_t *game, uint16_t now_ms);
bool snake_contains_point(const snake_game_t *game, int8_t x, int8_t y);

#endif // SNAKE_H
