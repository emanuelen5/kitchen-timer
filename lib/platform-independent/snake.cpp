#include "snake.h"

uint8_t snake_get_tail_dir(const snake_game_t *game, uint16_t idx)
{
    return (game->tail_dirs[idx / 4] >> ((idx % 4) * 2)) & 0x03;
}

namespace
{
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

    static bool is_occupied_by_body(const snake_game_t *game, snake_point_t point)
    {
        FOR_BODY_POSITION(game, i, pos)
        {
            if (points_equal(pos, point))
                return true;
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

            if (!is_occupied_by_body(game, candidate))
            {
                game->food = candidate;
                return;
            }
        }
    }

    static inline void snake_set_tail_dir(snake_game_t *game, uint16_t idx, uint8_t dir)
    {
        uint16_t byte_idx = idx / 4;
        uint8_t bit_offset = (idx % 4) * 2;
        game->tail_dirs[byte_idx] = (game->tail_dirs[byte_idx] & ~(0x03 << bit_offset)) | (dir << bit_offset);
    }
}

snake_point_t next_head_position(snake_point_t &head, snake_direction_t direction)
{
    snake_point_t next = {head.x, head.y};
    switch (direction)
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

    const int8_t playable_top_limit = 0;
    const int8_t playable_bottom_limit = snake_board_height - 1;
    const int8_t playable_left_limit = 0;
    const int8_t playable_right_limit = snake_board_width - 1;

    if (next.x < playable_left_limit)
        next.x = playable_right_limit;
    if (next.x > playable_right_limit)
        next.x = playable_left_limit;
    if (next.y < playable_top_limit)
        next.y = playable_bottom_limit;
    if (next.y > playable_bottom_limit)
        next.y = playable_top_limit;

    return next;
}

void snake_restart(snake_game_t *game, uint16_t seed)
{
    game->random_num_generator_state = seed == 0 ? 0xBEEF : seed;
    game->last_step_ms = 0;
    game->move_interval_ms = snake_initial_interval_ms;
    game->direction = SNAKE_RIGHT;
    game->status = SNAKE_RUNNING;
    game->turn_locked_until_step = false;

    game->head = {8, 8};
    game->length = 3;
    snake_set_tail_dir(game, 0, SNAKE_RIGHT);
    snake_set_tail_dir(game, 1, SNAKE_RIGHT);

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

static void move_snake(snake_game_t *game)
{
    snake_point_t next = next_head_position(game->head, game->direction);
    uint8_t new_dir = game->direction;

    // Shift all tail directions toward the end by one 2-bit slot
    uint8_t num_bytes = ((game->length - 1) + 3) / 4;
    for (int8_t i = num_bytes - 1; i > 0; i--)
        game->tail_dirs[i] = (game->tail_dirs[i] << 2) | (game->tail_dirs[i - 1] >> 6);
    game->tail_dirs[0] = (game->tail_dirs[0] << 2) | new_dir;

    game->head = next;
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
    snake_point_t next = next_head_position(game->head, game->direction);

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

    if (is_occupied_by_body(game, next))
    {
        game->status = SNAKE_GAME_OVER;
        return;
    }

    move_snake(game);
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
    return is_occupied_by_body(game, point);
}
