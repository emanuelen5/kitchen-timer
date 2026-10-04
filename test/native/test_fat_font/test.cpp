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

void test_valid_letters(void) {
    for (const char *c = "EHLMORSTUVW"; *c != '\0'; c++) {
        TEST_ASSERT_NOT_NULL_MESSAGE(get_bitmap(*c), "Expected non-null pointer for letter.");
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

void test_get_bitmap_for_letter(void) {
    const uint8_t* ptr_char_bitmap = get_bitmap('E');
    const uint8_t expected[] = {
        0b111111,
        0b110000,
        0b110000,
        0b111110,
        0b110000,
        0b110000,
        0b111111
    };

    for (int i = 0; i < 7; i++) {
        TEST_ASSERT_EQUAL_HEX8(expected[i], ptr_char_bitmap[i]);
    }
}

void test_lowercase_letters_use_the_uppercase_glyph(void) {
    TEST_ASSERT_EQUAL_PTR(get_bitmap('H'), get_bitmap('h'));
    TEST_ASSERT_EQUAL_PTR(get_bitmap('V'), get_bitmap('v'));
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_valid_digits);
    RUN_TEST(test_valid_letters);
    RUN_TEST(test_invalid_characters);
    RUN_TEST(test_get_bitmap_for_zero);
    RUN_TEST(test_get_bitmap_for_letter);
    RUN_TEST(test_lowercase_letters_use_the_uppercase_glyph);

    UNITY_END();
}
