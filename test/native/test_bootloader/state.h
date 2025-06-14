#include "bootloader_sm.h"

inline const char *state_to_str(state_t state)
{
    switch (state)
    {
    case STATE_INIT:
        return "STATE_INIT";
    case STATE_WAIT_FOR_PROGRAMMER:
        return "STATE_WAIT_FOR_PROGRAMMER";
    case STATE_WAIT_FOR_START_BYTE:
        return "STATE_WAIT_FOR_START_BYTE";
    case STATE_COMMAND:
        return "STATE_COMMAND";
    case STATE_LENGTH:
        return "STATE_LENGTH";
    case STATE_DATA:
        return "STATE_DATA";
    case STATE_CHECK_CHECKSUM:
        return "STATE_CHECK_CHECKSUM";
    case STATE_WRONG_CHECKSUM:
        return "STATE_WRONG_CHECKSUM";
    case STATE_RUN_COMMAND:
        return "STATE_RUN_COMMAND";
    case STATE_RETURN_STATUS:
        return "STATE_RETURN_STATUS";
    case STATE_READ_PAGE:
        return "STATE_READ_PAGE";
    case STATE_EXIT:
        return "STATE_EXIT";
    default:
        return "UNKNOWN";
    }
}

#define TEST_ASSERT_BOOTLOADER_IN_STATE(expected_state) TEST_ASSERT_EQUAL_STRING(state_to_str(expected_state), state_to_str(sm.state))
#define TEST_ASSERT_BOOTLOADER_IN_STATE_MESSAGE(expected_state, msg) TEST_ASSERT_EQUAL_STRING_MESSAGE(state_to_str(expected_state), state_to_str(sm.state), msg)