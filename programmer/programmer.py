#!/usr/bin/env python

import math
import sys
import time
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
from tqdm import tqdm
from tqdm.contrib import DummyTqdmFile


def print_stats(hexfile, pages: list[PageData]):
    page_offsets = [p.offset for p in pages]
    byte_count = sum(len(p.data) for p in pages)
    print(f"The hexfile {hexfile} contains the following:")
    print(
        f"{len(page_offsets)} pages of data {page_offsets} [start={min(page_offsets)}, end={max(page_offsets)}]"
    )
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
    packet = exchange_packets(
        s,
        create_signature_message(),
        packet_size(data_count=4),
        "signature",
    )
    actual_signature = packet.data[:3]
    if actual_signature != expected_signature:
        print(
            f"Signature doesn't match. Got {packet.data!r}. Wanted {expected_signature!r}",
            file=sys.stderr,
        )
        sys.exit(1)

    print(f"Signature check successful. Got {actual_signature.hex()}.")


response_data_size = packet_size(data_count=4)


def add_empty_pages_to_trigger_erase(pages: list[PageData]) -> list[PageData]:
    """Only pages on even offsets trigger an erase operation!"""
    new_pages = [p for p in pages]

    def new_empty_page(offset: int) -> PageData:
        return PageData(offset, bytes([0xFF] * page_size))

    page_offsets = {p.offset for p in pages}
    for offset in page_offsets:
        closest_erase_page = offset // 2 * 2
        if closest_erase_page not in page_offsets:
            new_pages.append(new_empty_page(closest_erase_page))

    return sorted(pages, key=lambda p: p.offset)


verbose = False
progress_cb = lambda p: None


def exchange_packets(
    s: Serial, w_packet: bytes, expected_read_length: int, phase: str
) -> Packet:
    write_length = s.write(w_packet)
    progress_cb(0.5)
    if verbose:
        print(f"-> {write_length} bytes: {w_packet.hex()}")
    if write_length != len(w_packet):
        print(
            f"ERROR ({phase}): Wrote {write_length} bytes, expected"
            f" {len(w_packet)} bytes.",
            file=sys.stderr,
        )
        sys.exit(1)

    data = s.read(expected_read_length)
    if not data and expected_read_length > 0:
        print(f"ERROR ({phase}): No data received from the device.", file=sys.stderr)
        sys.exit(1)
    elif len(data) < expected_read_length:
        print(
            f"ERROR ({phase}): Expected {expected_read_length} bytes, got"
            f" {len(data)} bytes.",
            file=sys.stderr,
        )
        sys.exit(1)
    if verbose:
        print(f"<- {len(data)} bytes: {data.hex()}")

    r_packet = Packet.from_bytes(data)
    errors = r_packet.get_any_validation_errors()
    if errors:
        print(f"ERROR ({phase}): {errors}", file=sys.stderr)
        sys.exit(1)

    if r_packet.ptype != PacketTypes.ack:
        print(
            f"ERROR ({phase}): The device returned an error packet {r_packet.ptype!r}",
            file=sys.stderr,
        )
        sys.exit(1)

    if verbose:
        print(f"SUCCESS ({phase}): {r_packet}")

    progress_cb(0.5)
    return r_packet


def create_pagedata(hexfile: Path) -> list[PageData]:
    pages = read_all_pagedata(hexfile)

    input_page_offsets = {p.offset for p in pages}
    pages = add_empty_pages_to_trigger_erase(pages)
    if verbose:
        inserted_pages = {p.offset for p in pages if p.offset not in input_page_offsets}
        print(
            f"Added {len(inserted_pages)} empty pages to trigger erase"
            f" : {inserted_pages}"
        )

    return pages


def read_and_dump_pages(s: Serial, path: Path, start: int, end: int):
    read_pages = []
    for page_offset in range(start // page_size, (end + page_size) // page_size):
        packet = exchange_packets(
            s,
            create_read_message(page_offset),
            packet_size(data_count=page_size + checksum_size),
            "read",
        )
        (actual_offset,) = unpack("<H", packet.data[:2])
        assert actual_offset == page_offset, (
            f"Expected page offset {page_offset}, got {actual_offset}"
        )
        read_pages.append(PageData(page_offset, packet.data[2:]))

    write_all_pagedata(path, read_pages)
    if verbose:
        print(f"Dumped data to {path}")


def write_pages(serial: Serial, pages: list[PageData]):
    for page in pages:
        exchange_packets(
            serial,
            create_write_message(page.offset, page.data),
            response_data_size,
            "write",
        )
        if verbose:
            print(f"Page {page.offset} written successfully.")


def boot_device(serial: Serial):
    exchange_packets(
        serial,
        create_boot_message(),
        response_data_size,
        "boot",
    )

    if verbose:
        print("Boot command sent successfully.")


def tqdm_with_print_monkeypatch(total: int) -> tqdm:
    """Create a tqdm progress bar that redirects all print statements to it.
    This makes sure that the progress bar is updated correctly and not broken
    in half by print statements."""
    std_out_and_std_err = sys.stdout, sys.stderr
    sys.stdout, sys.stderr = map(DummyTqdmFile, std_out_and_std_err)
    pbar = tqdm(total=total, file=std_out_and_std_err[0], dynamic_ncols=True)
    return pbar


def main():
    parser = ArgumentParser()
    parser.add_argument(
        "--hexfile", type=Path, help="Hex file to program to the device"
    )
    parser.add_argument("--verbose", "-v", action="store_true")
    parser.add_argument("--dry-run", "-n", action="store_true")
    parser.add_argument("--boot", action="store_true", help="Boot the device on finish")
    pg_read = parser.add_argument_group("Reading flash memory")
    pg_read.add_argument(
        "--dump",
        type=Path,
        help="Dump the raw data to a file. Do not write to the device.",
    )
    pg_read.add_argument(
        "--dump-start",
        type=lambda x: int(x, 0),
        default=0,
        help="Start offset for the dump",
    )
    pg_read.add_argument(
        "--dump-end",
        type=lambda x: int(x, 0),
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
    pg_serial.add_argument("--baudrate", default=62500, type=int)
    args = parser.parse_args()

    global verbose
    verbose = args.verbose

    if args.list_ports:
        print_available_comports()

    will_write = False
    pages = []
    if args.hexfile:
        pages = create_pagedata(args.hexfile)

        will_write = pages and not args.dry_run
        if args.verbose and not will_write:
            print_stats(args.hexfile, pages)

    if not args.port:
        parser.error("No serial port specified.")

    dump_size = (
        math.ceil((args.dump_end - args.dump_start + 1) / page_size) if args.dump else 0
    )
    write_size = len(pages) if will_write else 0
    will_boot = (args.hexfile or args.boot) and not args.dry_run

    pbar = tqdm_with_print_monkeypatch(
        total=1 + dump_size + write_size + (1 if will_boot else 0)
    )

    global progress_cb

    progress_cb = pbar.update
    serial = attempt_serial_connection(args.port, args.baudrate)
    time.sleep(0.1)  # Allow some time for the device to reset

    check_signature(serial, expected_signature=b"\x1e\x95\x0f")

    if pages and not args.dry_run:
        write_pages(serial, pages)

    if args.dump:
        read_and_dump_pages(
            serial,
            args.dump,
            args.dump_start,
            args.dump_end,
        )

    if will_boot:
        boot_device(serial)

    programmed_firmware = args.hexfile and not args.dry_run
    if programmed_firmware:
        print("Device is now running the new firmware.")

    pbar.close()


if __name__ == "__main__":
    main()
