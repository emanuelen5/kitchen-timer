#include <unity.h>

void setUp(void)
{
    // TODO: Set up the serial port
}

void tearDown(void)
{
}

void test_circular_buffer_empty_after_init()
{
    TEST_ASSERT_TRUE(true);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_circular_buffer_empty_after_init);

    UNITY_END();
}