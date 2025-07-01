# State machine

```mermaid
stateDiagram-v2
    state fork_state <<fork>>

    STATE_RETURN_STATUS: STATE_RETURN_STATUS <br> send 4 byte response
    STATE_WRONG_CHECKSUM: STATE_WRONG_CHECKSUM <br> set response status to _NACK_
    STATE_RUN_COMMAND: STATE_RUN_COMMAND <br> set response status to _ACK_

    [*] --> STATE_INIT : reset
    STATE_INIT --> STATE_WAIT_FOR_PROGRAMMER
    STATE_WAIT_FOR_PROGRAMMER --> STATE_EXIT : [timeout]
    STATE_WAIT_FOR_PROGRAMMER --> fork_state: [received byte]
    fork_state --> STATE_COMMAND : [_byte_ == START_BYTE]
    fork_state --> STATE_WAIT_FOR_START_BYTE : [else]
    STATE_WAIT_FOR_START_BYTE --> STATE_COMMAND : [received byte]
    STATE_COMMAND --> STATE_LENGTH : [received byte] <br> store as _command_
    STATE_LENGTH --> STATE_DATA : [received byte] <br> store as _length_
    STATE_DATA --> STATE_DATA : [received byte] <br> store in _data_ array
    STATE_DATA --> STATE_CHECK_CHECKSUM : [_length_+2 bytes of data received]
    STATE_CHECK_CHECKSUM --> STATE_WRONG_CHECKSUM : [❌ checksum]
    STATE_CHECK_CHECKSUM --> STATE_RUN_COMMAND : [✅ checksum]
    STATE_WRONG_CHECKSUM --> STATE_RETURN_STATUS
    STATE_RUN_COMMAND --> STATE_RETURN_STATUS: [else]
    STATE_RUN_COMMAND --> STATE_READ_PAGE : [_command_ == COMMAND_READ_PAGE]
    STATE_READ_PAGE --> STATE_WAIT_FOR_START_BYTE : send page response
    STATE_RETURN_STATUS --> STATE_WAIT_FOR_START_BYTE : [_command_ != COMMAND_BOOT]
    STATE_RETURN_STATUS --> STATE_EXIT : [_command_ == COMMAND_BOOT]
    STATE_EXIT --> [*]
```

- The bootloader state machine starts at `STATE_INIT` ⚡, *resets*, and waits for the programmer.
- ⏳ If no programmer sends any command within a timeout, it automatically *exits*.
- It transitions through states as it receives bytes: start byte, command, length, and data.
- After collecting data, it checks the checksum.
- If the checksum is wrong, the device responds with a *NAK* ❌; if correct, it *runs the command* and responds with an *ACK* ✅.
- Most commands return a status; `COMMAND_READ_PAGE` 📖 *sends a page response*.
- In `STATE_RETURN_STATUS`, the bootloader sends a response packet back to the programmer.
- In `STATE_READ_PAGE`, the bootloader sends the requested page data as a response.
- If the command is `COMMAND_BOOT` 🚀, the state machine *exits*; otherwise, it loops back to wait for the next command.
