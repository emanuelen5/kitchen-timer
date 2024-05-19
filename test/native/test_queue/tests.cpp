#include <unity.h>
#include "uint8-queue.h"

uint8_queue_t q;

void setUp(void)
{
    q = {};
    init_queue(&q);
}

void tearDown(void)
{
}

void test_is_empty_after_init()
{
    uint8_queue_t q2 = {};
    init_queue(&q2);
    TEST_ASSERT_TRUE(queue_is_empty(&q2));
    TEST_ASSERT_FALSE(queue_is_full(&q));
}

void test_can_take_an_item()
{
    add_to_queue(&q, 0);
    TEST_ASSERT_FALSE(queue_is_empty(&q));
    TEST_ASSERT_FALSE(queue_is_full(&q));
}

void test_can_take_max_items()
{
    int queue_size = 0;
    while (!queue_is_full(&q))
    {
        add_to_queue(&q, 0);
        queue_size++;
    }
    TEST_ASSERT_EQUAL(QUEUE_SIZE, queue_size);
}

void make_queue_full(uint8_queue_t *q)
{
    while (!queue_is_full(q))
    {
        add_to_queue(q, 0);
    }
}

void test_can_remove_items()
{
    make_queue_full(&q);
    dequeue(&q);
    TEST_ASSERT_FALSE(queue_is_full(&q));
}

void test_empty_shall_return_invalid_dequeue()
{
    TEST_ASSERT_TRUE(queue_is_empty(&q));
    dequeue_return_t v = dequeue(&q);
    TEST_ASSERT_FALSE(v.is_valid);
}

void test_value_can_be_retrieved_from_queue()
{
    add_to_queue(&q, 0xaa);
    dequeue_return_t v = dequeue(&q);
    TEST_ASSERT_TRUE(v.is_valid);
    TEST_ASSERT_EQUAL(0xaa, v.value);
}

void test_values_are_returned_in_fifo_order()
{
    add_to_queue(&q, 1);
    add_to_queue(&q, 2);
    dequeue_return_t v = dequeue(&q);
    TEST_ASSERT_TRUE(v.is_valid);
    TEST_ASSERT_EQUAL(1, v.value);
}

void test_when_adding_to_a_full_queue_the_overflow_flag_shall_be_set()
{
    make_queue_full(&q);
    add_to_queue(&q, 0xff);
    TEST_ASSERT_TRUE(has_queue_overflowed(&q));
}

void test_when_overflow_flag_has_been_read_it_shall_be_cleared()
{
    make_queue_full(&q);
    add_to_queue(&q, 0xff);
    has_queue_overflowed(&q);
    TEST_ASSERT_FALSE(has_queue_overflowed(&q));
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_is_empty_after_init);
    RUN_TEST(test_can_take_an_item);
    RUN_TEST(test_can_take_max_items);
    RUN_TEST(test_can_remove_items);
    RUN_TEST(test_empty_shall_return_invalid_dequeue);
    RUN_TEST(test_value_can_be_retrieved_from_queue);
    RUN_TEST(test_values_are_returned_in_fifo_order);
    RUN_TEST(test_when_adding_to_a_full_queue_the_overflow_flag_shall_be_set);
    RUN_TEST(test_when_overflow_flag_has_been_read_it_shall_be_cleared);

    UNITY_END();
}
