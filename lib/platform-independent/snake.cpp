#include "snake.h"

namespace
{
    constexpr int8_t snake_board_width = 16;
    constexpr int8_t snake_board_height = 16;
    constexpr uint8_t snake_initial_length = 3;
    constexpr uint16_t snake_initial_interval_ms = 325;
    constexpr uint16_t snake_min_interval_ms = 90;
    constexpr uint16_t snake_speedup_ms = 12;

    static uint16_t random_num_generator(snake_game_t *game)
    {
        uint16_t state = game->random_num_generator_state;
        if (state == 0) // Prevents the rng to get stuck in zero.
            state = 0xACE1;

        // Generating the rundom number:
        const uint16_t least_significant_bit = state & 1U;
        state >>= 1;
        if (least_significant_bit != 0)
            state ^= 0xB400U;

        game->random_num_generator_state = state;
        return state;
    }

    static bool points_equal(snake_point_t a, snake_point_t b)
    {
        return a.x == b.x && a.y == b.y;
    }

    static bool is_occupied_by_body(const snake_game_t *game, snake_point_t point, bool head_included)
    {
        uint16_t i = 0;
        if (!head_included)
        {
            i = 1;
        }

        for (; i < game->length; i++)
        {
            if (points_equal(game->body[i], point))
            {
                return true;
            }
        }

        return false;
    }

    static void spawn_food(snake_game_t *game)
    {
        const uint16_t cell_count = snake_board_width * snake_board_height;

        const uint16_t start_index = random_num_generator(game) % cell_count;
        for (uint16_t offset = 0; offset < cell_count; offset++)
        {
            const uint16_t index = (start_index + offset) % cell_count;
            snake_point_t candidate = {
                (int8_t)(index % snake_board_width),
                (int8_t)(index / snake_board_width),
            };

            if (!is_occupied_by_body(game, candidate, true))
            {
                game->food = candidate;
                return;
            }
        }
    }

    static snake_point_t next_head_position(const snake_game_t *game)
    {
        snake_point_t next = game->body[0];
        switch (game->direction)
        {
        case SNAKE_UP:
            next.y--;
            break;
        case SNAKE_RIGHT:
            next.x++;
            break;
        case SNAKE_DOWN:
            next.y++;
            break;
        case SNAKE_LEFT:
            next.x--;
            break;
        default:
            break;
        }

        return next;
    }
}

void snake_restart(snake_game_t *game, uint16_t seed)
{
    game->random_num_generator_state = seed == 0 ? 0xBEEF : seed;
    game->last_step_ms = 0;
    game->move_interval_ms = snake_initial_interval_ms;
    game->length = snake_initial_length;
    game->direction = SNAKE_RIGHT;
    game->status = SNAKE_RUNNING;
    game->turn_locked_until_step = false;

    game->body[0] = {8, 8};
    game->body[1] = {7, 8};
    game->body[2] = {6, 8};

    spawn_food(game);
}

void snake_turn_left(snake_game_t *game)
{
    if (game->turn_locked_until_step)
    {
        return;
    }

    game->direction = (snake_direction_t)((game->direction + SNAKE_DIRECTION_COUNT - 1) % SNAKE_DIRECTION_COUNT);
    game->turn_locked_until_step = true;
}

void snake_turn_right(snake_game_t *game)
{
    if (game->turn_locked_until_step)
    {
        return;
    }

    game->direction = (snake_direction_t)((game->direction + 1) % SNAKE_DIRECTION_COUNT);
    game->turn_locked_until_step = true;
}


static void move_snake(snake_game_t *game, snake_point_t *next)
{
    for (int16_t i = game->length - 1; i > 0; i--)
    {
        game->body[i] = game->body[i - 1];
    }

    game->body[0] = *next;

    const int8_t playable_top_limit = 0;
    const int8_t playable_bottom_limit = snake_board_height - 1;
    const int8_t playable_left_limit = 0;
    const int8_t playable_right_limit = snake_board_width - 1;

    if (game->body[0].x < playable_left_limit)
        game->body[0].x = playable_right_limit;
    if (game->body[0].x > playable_right_limit)
        game->body[0].x = playable_left_limit;
    if (game->body[0].y < playable_top_limit)
        game->body[0].y = playable_bottom_limit;
    if (game->body[0].y > playable_bottom_limit)
        game->body[0].y = playable_top_limit;
}

void service_snake_game(snake_game_t *game, uint16_t now_ms)
{
    if (game->status != SNAKE_RUNNING)
    {
        return;
    }

    if ((uint16_t)(now_ms - game->last_step_ms) < game->move_interval_ms)
    {
        return;
    }

    game->last_step_ms = now_ms;
    snake_point_t next = next_head_position(game);

    const bool ate_food = points_equal(next, game->food);

    const uint16_t cell_count = snake_board_width * snake_board_height;

    if (ate_food)
    {
        game->length += 1;

        if (game->length >= cell_count)
        {
            game->status = SNAKE_WON;
            return;
        }
    }

    if (is_occupied_by_body(game, next, true))
    {
        game->status = SNAKE_GAME_OVER;
        return;
    }

    move_snake(game, &next);
    game->turn_locked_until_step = false;

    if (ate_food)
    {
        if (game->move_interval_ms > snake_min_interval_ms)
        {
            uint16_t next_interval = game->move_interval_ms - snake_speedup_ms;
            game->move_interval_ms = next_interval < snake_min_interval_ms ? snake_min_interval_ms : next_interval;
        }

        spawn_food(game);
    }
}

bool snake_contains_point(const snake_game_t *game, int8_t x, int8_t y)
{
    snake_point_t point = {x, y};
    return is_occupied_by_body(game, point, true);
}
