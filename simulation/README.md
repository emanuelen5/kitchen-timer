# Firmware simulation with Simavr

To connect to the simulator, do the following:

## Build Simavr simulation program

> [!NOTE]
> You must have built the bootloader firmware first!
> ```bash
> pio run -e bootloader
> ```

Then you can build the simulator

```bash
make
```

Run the simulator

```bash
build/simulator ../.pio/build/bootloader/firmware.hex
```

### Debug

If you pass the `-d` flag to the simulator, it will start a gdb debug server that you can attach to.

Then you can start a debug session with either
  1. From the VScode GUI <br>
     By using the launch configuration called "Attach to AVR gdbserver".
  2. From the command line <br>
     ```bash
     avr-gdb .pio/build/bootloader/firmware.elf
     ```
     And then run `target remote :1234` to attach to the debug session.

## License

This simulator uses Simavr, which is licensed as GPL-3.0. Thus, the whole
simulator is also licensed as GPL-3.0.
