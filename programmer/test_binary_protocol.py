from struct import pack

from programmer.binary_protocol import (
    Packet,
    PacketTypes,
    crc16,
    create_write_message,
    packet,
    start_byte,
)
from programmer.intel_hexfile import page_size


def test_build_version_packet():
    assert b"\x03\x01\x00\x80\x50" == packet(PacketTypes.version)


def test_build_boot_packet():
    assert b"\x03\x03\x00\x81\x30" == packet(PacketTypes.boot)


def test_build_write_packet():
    assert b"\x03\x02\x82\xaa\x00" + bytes(
        page_size
    ) + b"z\xa3" == create_write_message(page=0xAA, data=bytes(page_size))


def test_crc16():
    assert 16575 == crc16(b"\x00")
    assert 45057 == crc16(b"\x00\x00")


def test_crc16_cancels_itself():
    assert 0 == crc16(b"\x00" + pack("<H", 16575))
    assert 0 == crc16(b"\x00\x00" + pack("<H", 45057))


def append_checksum(data: bytes) -> bytes:
    return data + pack("<H", crc16(data))


def test_packet():
    raw_data = append_checksum(start_byte + b"\x01\x02\x03\x04")
    p = Packet.from_bytes(raw_data)
    assert p.get_any_validation_errors() == ""


def test_read_back_raw():
    raw_data = append_checksum(start_byte + b"\x01\x02\x03\x04")
    p = Packet.from_bytes(raw_data)
    assert p.raw == raw_data


def test_dissect_packet():
    raw_data = append_checksum(start_byte + b"\x01\x02\x03\x04")
    p = Packet.from_bytes(raw_data)
    assert p.ptype == 0x01
    assert p.length == 0x02
    assert p.data == b"\x03\x04"
