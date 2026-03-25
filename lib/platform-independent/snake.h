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

typedef struct
{
    snake_point_t body[16 * 16];
    snake_point_t food;
    uint16_t last_step_ms;
    uint16_t move_interval_ms;
    uint16_t random_num_generator_state;
    uint16_t length;
    snake_direction_t direction;
    snake_status_t status;
    bool turn_locked_until_step;
} snake_game_t;

void snake_turn_left(snake_game_t *game);
void snake_turn_right(snake_game_t *game);
void snake_toggle_pause(snake_game_t *game);
void snake_restart(snake_game_t *game, uint16_t seed);
void service_snake_game(snake_game_t *game, uint16_t now_ms);
bool snake_contains_point(const snake_game_t *game, int8_t x, int8_t y);

#endif // SNAKE_H
