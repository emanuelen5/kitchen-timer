#!/usr/bin/env python

import sys
from argparse import ArgumentParser
from pathlib import Path
from struct import unpack

from binary_protocol import (
    Packet,
    PacketTypes,
    checksum_size,
    create_boot_message,
    create_read_message,
    create_signature_message,
    create_write_message,
    packet_size,
    page_size,
)
from intel_hexfile import PageData, read_all_pagedata, write_all_pagedata
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

    data = s.read(packet_size(data_count=4))
    print("data", data.hex())
    p = Packet.from_bytes(data)
    print("Packet", p)

    errors = p.get_any_validation_errors()
    assert not errors, f"Got validation errors for packet: {errors}"  # throw and retry
    assert p.ptype == PacketTypes.ack, f"Expected ACK packet, got {p.ptype!r}"
    assert (  # this is really bad
        p.data[:3] == expected_signature
    ), f"Signature doesn't match. Got {p.data!r}. Wanted {expected_signature!r}"


response_data_size = packet_size(data_count=4 + checksum_size)


def main():
    parser = ArgumentParser()
    parser.add_argument("hexfile", type=Path)
    parser.add_argument("--verbose", "-v", action="store_true")
    parser.add_argument("--dry-run", "-n", action="store_true")
    parser.add_argument("--dump", type=Path, help="Dump the raw data to a file")
    parser.add_argument(
        "--dump-start", type=int, default=0, help="Start offset for the dump"
    )
    parser.add_argument(
        "--dump-end",
        type=int,
        default=32 * 1024,
        help="End offset for the dump (exclusive)",
    )
    pg_serial = parser.add_argument_group("Serial connection")
    pge_port = pg_serial.add_mutually_exclusive_group()
    pge_port.add_argument(
        "--list-ports",
        action="store_true",
        help="The serial port name to connect to",
    )
    pge_port.add_argument("--port", help="The serial port name to connect to")
    pg_serial.add_argument("--baudrate", default=9600)
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

    check_signature(serial, expected_signature=b"\x1e\x95\x0f")

    if args.dump:
        read_pages = []
        for page_offset in range(
            args.dump_start // page_size, (args.dump_end + page_size - 1) // page_size
        ):
            serial.write(create_read_message(page_offset))
            data = serial.read(packet_size(data_count=page_size + checksum_size))
            print("data", data.hex())
            packet = Packet.from_bytes(data)
            print("Packet", packet)
            errors = packet.get_any_validation_errors()
            if errors:
                print(f"ERROR (page {page_offset}): {errors}", file=sys.stderr)
                sys.exit(1)
            if packet.ptype != PacketTypes.ack:
                print(
                    f"ERROR (page {page_offset}):"
                    f" The device returned an error code {packet.ptype!r}",
                    file=sys.stderr,
                )
                sys.exit(1)
            assert unpack("<H", packet.data[:2]) == (page_offset,), (
                f"Expected page offset {page_offset}, got {unpack('<H', packet.data[:2])}"
            )
            read_pages.append(PageData(page_offset, packet.data[2:]))

        write_all_pagedata(args.dump, read_pages)
        print(f"Dumped data to {args.dump}")
        sys.exit(0)

    for page in pages:
        serial.write(create_write_message(page.offset, page.data))
        data = serial.read(response_data_size)
        packet = Packet.from_bytes(data)
        errors = packet.get_any_validation_errors()
        if errors:
            print(f"ERROR (page {page.offset}): {errors}", file=sys.stderr)
            sys.exit(1)

        if packet.ptype != PacketTypes.ack:
            print(
                f"ERROR (page {page.offset}):"
                f" The device returned an error code {packet.ptype!r}",
                file=sys.stderr,
            )
            sys.exit(1)

        if args.verbose:
            print(f"Page {page.offset} written successfully.")

    serial.write(create_boot_message())
    data = serial.read(response_data_size)
    packet = Packet.from_bytes(data)
    errors = packet.get_any_validation_errors()
    if errors:
        print(f"ERROR: {errors}", file=sys.stderr)
        sys.exit(1)
    if packet.ptype != PacketTypes.ack:
        print(
            f"ERROR: The device returned an error code {packet.ptype!r}",
            file=sys.stderr,
        )
        sys.exit(1)

    if args.verbose:
        print(
            "Boot command sent successfully. Device should now be running the new firmware."
        )


if __name__ == "__main__":
    main()
