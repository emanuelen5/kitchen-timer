#!/usr/bin/env python

import sys
from argparse import ArgumentParser
from pathlib import Path

from binary_protocol import (
    Packet,
    PacketTypes,
    create_boot_message,
    create_signature_message,
    create_write_message,
    packet_size,
    start_byte,
)
from intel_hexfile import read_all_pagedata
from serial import Serial, SerialException
from serial.tools.list_ports import comports


def print_stats(args, pages):
    page_offsets = [p.offset for p in pages]
    byte_count = sum(len(p.data) for p in pages)
    print(f"The hexfile {args.hexfile} contains the following:")
    print(f"{len(page_offsets)} pages of data {page_offsets}")
    print(f"with a total of {byte_count} bytes")


def print_available_comports():
    print("The following ports are available:")
    for p in sorted(comports(), key=lambda p: p.device):
        print(f"  - {p.device} ({p.hwid})")


def attempt_serial_connection(portname: str, baudrate: int) -> Serial:
    try:
        return Serial(portname, baudrate, timeout=0.5)
    except SerialException:
        print(
            f"ERROR: Failed to get a hold of the serial port {portname}.",
            file=sys.stderr,
        )
        print_available_comports()
        sys.exit(1)


def check_signature(s: Serial, expected_signature: bytes = b"\x1e\x95\x0f"):
    s.reset_input_buffer()
    s.write(create_signature_message())

    data = s.read(packet_size(data_count=0))
    print("data", data.hex())
    p = Packet.from_bytes(data)
    print("Packet", p)

    errors = p.get_any_validation_errors()
    assert not errors, f"Got validation errors for packet: {errors}"  # throw and retry
    assert p.ptype is PacketTypes.ack  # throw and retry
    assert (  # this is really bad
        p.data == expected_signature
    ), f"Signature doesn't match. Got {p.data!r}. Wanted {expected_signature!r}"


def main():
    parser = ArgumentParser()
    parser.add_argument("hexfile", type=Path)
    parser.add_argument("--verbose", "-v", action="store_true")
    parser.add_argument("--dry-run", "-n", action="store_true")
    pg_serial = parser.add_argument_group("Serial connection")
    pge_port = pg_serial.add_mutually_exclusive_group()
    pge_port.add_argument(
        "--list-ports",
        action="store_true",
        help="The serial port name to connect to",
    )
    pge_port.add_argument("--port", help="The serial port name to connect to")
    pg_serial.add_argument("--baudrate", default=125000)
    args = parser.parse_args()

    pages = read_all_pagedata(args.hexfile)

    if args.dry_run or args.verbose:
        print_stats(args, pages)

    if args.list_ports:
        print_available_comports()
        return

    if args.dry_run:
        sys.exit(0)

    serial = attempt_serial_connection(args.port, args.baudrate)
    serial.write(b"asd")


if __name__ == "__main__":
    main()
