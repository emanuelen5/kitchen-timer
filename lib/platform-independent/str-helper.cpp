#include "str-helper.h"

uint8_t uint16_string_length(uint16_t num)
{
    uint8_t length = 1; // Always at least one digit (even when zero)
    for (uint16_t num_remainder = num / 10; num_remainder != 0; length++)
    {
        num_remainder /= 10;
    }
    return length;
}

uint8_t write_int_into_string(int16_t num, char *str)
{
    uint8_t written_chars = 0;
    bool is_negative = num < 0;
    const uint16_t absolute_num = is_negative ? -num : num;
    uint8_t number_length = uint16_string_length(absolute_num);

    char *number_start;
    if (is_negative)
    {
        *str = '-';
        number_start = str + 1;
        written_chars++;
    }
    else
    {
        number_start = str;
    }
    number_start[number_length] = '\0';

    uint16_t num_remainder = absolute_num;
    for (int8_t char_offset = number_length - 1; char_offset >= 0; char_offset--)
    {
        uint8_t number_character = (num_remainder % 10) + '0';
        number_start[char_offset] = number_character;
        written_chars++;
        num_remainder /= 10;
    }

    return written_chars;
}

bool str_equals(const char* a, const char* b)
{
    if (a == 0 || b == 0) {
        return false;
    }

    while (*a && *b) {
        if (*a != *b) {
            return false;
        }
        a++;
        b++;
    }

    return (*a == '\0' && *b == '\0'); //// Check to make sure there are no more chars in any of them.
}
