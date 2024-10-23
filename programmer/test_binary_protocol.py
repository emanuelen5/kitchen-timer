from struct import pack

from programmer.binary_protocol import PacketTypes, crc16, packet, write_packet
from programmer.intel_hexfile import page_size


def test_build_version_packet():
    assert b"\x03\x01\x00\x80\x50" == packet(PacketTypes.version)


def test_build_boot_packet():
    assert b"\x03\x03\x00\x81\x30" == packet(PacketTypes.boot)


def test_build_write_packet():
    assert b"\x03\x02B\xaa\x00" + bytes(page_size) + b"c#" == write_packet(
        page=0xAA, data=bytes(page_size)
    )


def test_crc16():
    assert 16575 == crc16(b"\x00")
    assert 45057 == crc16(b"\x00\x00")


def test_crc16_cancels_itself():
    assert 0 == crc16(b"\x00" + pack("<H", 16575))
    assert 0 == crc16(b"\x00\x00" + pack("<H", 45057))
