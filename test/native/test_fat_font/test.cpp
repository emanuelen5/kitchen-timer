#include <unity.h>
#include "fat_font.h"

void setUp(void){}
void tearDown(void) {}

void test_valid_digits(void) {
    for (char c = '0'; c <= '9'; c++) {
        const uint8_t* glyph = get_bitmap(c);
        TEST_ASSERT_NOT_NULL_MESSAGE(glyph, "Expected non-null pointer for digit.");
    }
}

void test_invalid_characters(void) {
    TEST_ASSERT_NULL(get_bitmap('a'));
    TEST_ASSERT_NULL(get_bitmap(' '));
    TEST_ASSERT_NULL(get_bitmap('/'));
    TEST_ASSERT_NULL(get_bitmap(':'));
}

void test_get_bitmap_for_zero(void) {
    const uint8_t* ptr_char_bitmap = get_bitmap('0');
    const uint8_t expected[] = {
        0b011110,
        0b110011,
        0b110111,
        0b111111,
        0b111011,
        0b110011,
        0b011110
    };

    for (int i = 0; i < 7; i++) {
        TEST_ASSERT_EQUAL_HEX8(expected[i], ptr_char_bitmap[i]);
    }
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_valid_digits);
    RUN_TEST(test_invalid_characters);
    RUN_TEST(test_get_bitmap_for_zero);

    UNITY_END();
}
