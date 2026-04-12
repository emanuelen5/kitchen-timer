#include <unity.h>

#include "snake.h"

snake_game_t game;

void setUp(void)
{
    snake_restart(&game, 1);
}

void tearDown(void)
{
}

void test_initializes_running_snake_with_food_not_on_body(void)
{
    TEST_ASSERT_EQUAL(SNAKE_RUNNING, game.status);
    TEST_ASSERT_EQUAL(3, game.length);
    TEST_ASSERT_FALSE(snake_contains_point(&game, game.food.x, game.food.y));
}

void test_moves_once_after_interval_has_elapsed(void)
{
    service_snake_game(&game, 324);
    TEST_ASSERT_EQUAL(8, game.head.x);
    TEST_ASSERT_EQUAL(8, game.head.y);

    service_snake_game(&game, 325);
    TEST_ASSERT_EQUAL(9, game.head.x);
    TEST_ASSERT_EQUAL(8, game.head.y);
}

void test_turns_relative_to_current_direction(void)
{
    snake_turn_left(&game);
    service_snake_game(&game, 325);

    TEST_ASSERT_EQUAL(8, game.head.x);
    TEST_ASSERT_EQUAL(7, game.head.y);
}

void test_ignores_second_turn_before_next_step(void)
{
    snake_turn_left(&game);
    snake_turn_left(&game);
    service_snake_game(&game, 325);

    TEST_ASSERT_EQUAL(8, game.head.x);
    TEST_ASSERT_EQUAL(7, game.head.y);
}

void test_hitting_wall_continues_game(void)
{
    for (uint16_t now = 325; now <= 2600; now += 325)
    {
        service_snake_game(&game, now);
    }

    TEST_ASSERT_EQUAL(SNAKE_RUNNING, game.status);
}

void test_eating_food_grows_snake(void)
{
    game.food = {9, 8};

    service_snake_game(&game, 325);

    TEST_ASSERT_EQUAL(4, game.length);
    TEST_ASSERT_EQUAL(9, game.head.x);
    TEST_ASSERT_EQUAL(8, game.head.y);
}

void test_eating_food_after_wrapping_through_wall(void)
{
    game.food = {0, 8};
    game.head = {15, 8};
    game.length = 1;
    game.direction = SNAKE_RIGHT;

    service_snake_game(&game, 325);

    TEST_ASSERT_EQUAL(0, game.head.x);
    TEST_ASSERT_EQUAL(8, game.head.y);
    TEST_ASSERT_EQUAL(2, game.length);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_initializes_running_snake_with_food_not_on_body);
    RUN_TEST(test_moves_once_after_interval_has_elapsed);
    RUN_TEST(test_turns_relative_to_current_direction);
    RUN_TEST(test_ignores_second_turn_before_next_step);
    RUN_TEST(test_hitting_wall_continues_game);
    RUN_TEST(test_eating_food_grows_snake);
    RUN_TEST(test_eating_food_after_wrapping_through_wall);

    return UNITY_END();
}
