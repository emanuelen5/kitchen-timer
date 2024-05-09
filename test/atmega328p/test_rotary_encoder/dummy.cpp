#include <unity.h>
#include "unity_config.h"

void setUp(void)
{
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
