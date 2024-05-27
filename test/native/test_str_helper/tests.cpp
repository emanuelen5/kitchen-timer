#include <unity.h>
#include "str-helper.h"
#include "string.h"

char buf[8] = {0};

void setUp(void)
{
    memset(buf, 0, 8);
}

void tearDown(void)
{
}

void test_number_length_one_digit(void)
{
    TEST_ASSERT_EQUAL(1, uint16_string_length(0));
    TEST_ASSERT_EQUAL(1, uint16_string_length(9));
}

void test_number_length_two_digits(void)
{
    TEST_ASSERT_EQUAL(2, uint16_string_length(10));
    TEST_ASSERT_EQUAL(2, uint16_string_length(19));
}

void test_number_length_max_digits(void)
{
    TEST_ASSERT_EQUAL(5, uint16_string_length(0xFFFF));
}

void test_convert_zero(void)
{
    uint8_t written = write_int_into_string(0, buf);
    TEST_ASSERT_EQUAL(1, written);
    TEST_ASSERT_EQUAL_STRING("0", buf);
}

void test_convert_9(void)
{
    uint8_t written = write_int_into_string(9, buf);
    TEST_ASSERT_EQUAL(1, written);
    TEST_ASSERT_EQUAL_STRING("9", buf);
}

void test_convert_negative_one(void)
{
    uint8_t written = write_int_into_string(-1, buf);
    TEST_ASSERT_EQUAL(2, written);
    TEST_ASSERT_EQUAL_STRING("-1", buf);
}

void test_convert_10(void)
{
    uint8_t written = write_int_into_string(10, buf);
    TEST_ASSERT_EQUAL(2, written);
    TEST_ASSERT_EQUAL_STRING("10", buf);
}

void test_convert_12(void)
{
    uint8_t written = write_int_into_string(12, buf);
    TEST_ASSERT_EQUAL(2, written);
    TEST_ASSERT_EQUAL_STRING("12", buf);
}

void test_convert_123(void)
{
    uint8_t written = write_int_into_string(123, buf);
    TEST_ASSERT_EQUAL(3, written);
    TEST_ASSERT_EQUAL_STRING("123", buf);
}

void test_convert_1234(void)
{
    uint8_t written = write_int_into_string(1234, buf);
    TEST_ASSERT_EQUAL(4, written);
    TEST_ASSERT_EQUAL_STRING("1234", buf);
}

void test_convert_12345(void)
{
    uint8_t written = write_int_into_string(12345, buf);
    TEST_ASSERT_EQUAL(5, written);
    TEST_ASSERT_EQUAL_STRING("12345", buf);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_number_length_one_digit);
    RUN_TEST(test_number_length_two_digits);
    RUN_TEST(test_number_length_max_digits);
    RUN_TEST(test_convert_zero);
    RUN_TEST(test_convert_9);
    RUN_TEST(test_convert_negative_one);
    RUN_TEST(test_convert_10);
    RUN_TEST(test_convert_12);
    RUN_TEST(test_convert_123);
    RUN_TEST(test_convert_1234);
    RUN_TEST(test_convert_12345);

    UNITY_END();
}
