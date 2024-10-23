import enum
from struct import pack

start_byte = b"\x03"


def crc16_update(crc, a):
    crc ^= a
    for _ in range(8):
        if crc & 1:
            crc = (crc >> 1) ^ 0xA001
        else:
            crc = crc >> 1
    return crc


def crc16(values: bytes):
    accumulator = 0xFFFF
    for v in values:
        accumulator = crc16_update(accumulator, v)
    return accumulator


class PacketTypes(enum.IntEnum):
    signature = 0x00
    version = 0x01
    write = 0x02
    boot = 0x03


def packet(ptype: PacketTypes, data: bytes = None) -> bytes:
    if data is None:
        data = b""

    payload = start_byte + bytes([ptype, len(data)]) + bytes(data)
    checksum = crc16(payload)

    return payload + pack("<H", checksum)


def write_packet(page: int, data: bytes) -> bytes:
    return packet(PacketTypes.write, pack("<H", page) + data)
