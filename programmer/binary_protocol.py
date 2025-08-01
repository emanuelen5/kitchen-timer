import enum
from dataclasses import dataclass
from struct import pack, unpack
from typing import Self

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
    # To the AVR
    signature = 0x00
    version = 0x01
    write = 0x02
    boot = 0x03
    read = 0x04
    # From the AVR
    ack = 0x10
    nack = 0x11
    unknown_command = 0x12


@dataclass
class Packet:
    """
    Message / Packet format:
        [start-byte:1] [packet-type:1] [length:1] [data:x] [checksum:2]

    * Total length = 5 + x
    * Checksum is big-endian.
    """

    start: bytes
    ptype: PacketTypes
    length: int
    data: bytes
    checksum: int

    @classmethod
    def from_bytes(cls, d: bytes) -> Self:
        assert len(d) >= 5
        data = d[3:-2]
        crc, *_ = unpack("<H", d[-2:])
        inst = cls(
            start=d[0:1],
            ptype=PacketTypes(d[1]),
            length=d[2],
            data=data,
            checksum=crc,
        )
        return inst

    def get_any_validation_errors(self) -> str:
        errors = []
        if self.start != start_byte:
            errors.append("Start byte isn't valid")
        if len(self.data) != self.length:
            errors.append(
                f"We received wrong amount of data."
                f" Got {len(self.data)} but wanted {self.length}"
            )
        if crc16(self.raw) != 0:
            expected_crc = crc16(self.raw[:-2])
            errors.append(
                f"The checksum didn't match. Expected {expected_crc} (0x{expected_crc:04x})"
                f" but got {self.checksum} (0x{self.checksum:04x})"
            )

        return "".join("\n  - " + e for e in errors)

    @property
    def raw(self) -> bytes:
        return (
            self.start
            + pack("<BB", self.ptype, self.length)
            + self.data
            + pack("<H", self.checksum)
        )

    def __str__(self) -> str:
        packet_type_str = f"{self.ptype.name} ({self.ptype.value})"
        match self.ptype:
            case PacketTypes.signature:
                return f"Packet(type={packet_type_str}, data={self.data.hex()})"
            case PacketTypes.version:
                return f"Packet(type={packet_type_str}, data={self.data.hex()})"
            case PacketTypes.write:
                return f"Packet(type={packet_type_str}, page={unpack('<H', self.data[:2])[0]}, data={self.data[2:].hex()})"
            case PacketTypes.boot:
                return f"Packet(type={packet_type_str})"
            case PacketTypes.read:
                return f"Packet(type={packet_type_str}, page={unpack('<H', self.data[:2])[0]})"
            case PacketTypes.ack:
                return f"Packet(type={packet_type_str})"
            case PacketTypes.nack:
                return f"Packet(type={packet_type_str})"
            case PacketTypes.unknown_command:
                return f"Packet(type={packet_type_str})"
            case _:
                return f"Packet(type={packet_type_str}, data={self.data.hex()})"


def packet(ptype: PacketTypes, data: bytes = b"") -> bytes:
    payload = start_byte + bytes([ptype, len(data)]) + bytes(data)
    checksum = crc16(payload)

    return payload + pack("<H", checksum)


def create_signature_message() -> bytes:
    return packet(PacketTypes.signature)


def create_boot_message() -> bytes:
    return packet(PacketTypes.boot)


def create_write_message(page: int, data: bytes) -> bytes:
    return packet(PacketTypes.write, pack("<H", page) + data)


def create_read_message(page: int) -> bytes:
    return packet(PacketTypes.read, pack("<H", page))


def packet_size(data_count: int) -> int:
    return 3 + data_count + 2


page_size = 128
checksum_size = 2
