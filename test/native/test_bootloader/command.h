#include "bootloader_sm.h"

inline const char *response_to_string(uint8_t resp)
{
    switch (resp)
    {
    case resp_ack:
        return "resp_ack";
    case resp_nak:
        return "resp_nak";
    case resp_data_unknown_command:
        return "resp_data_unknown_command";
    }
    return "Unknown response";
}
