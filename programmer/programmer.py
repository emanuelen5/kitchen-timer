#!/usr/bin/env python

import sys
from argparse import ArgumentParser
from pathlib import Path

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
        return Serial(portname, baudrate)
    except SerialException:
        print(
            f"ERROR: Failed to get a hold of the serial port {portname}.",
            file=sys.stderr,
        )
        print_available_comports()
        sys.exit(1)


def main():
    parser = ArgumentParser()
    parser.add_argument("hexfile", type=Path)
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

    print_stats(args, pages)

    if args.list_ports:
        print_available_comports()
        return

    serial = attempt_serial_connection(args.port, args.baudrate)


if __name__ == "__main__":
    main()
