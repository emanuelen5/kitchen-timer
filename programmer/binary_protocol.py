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
    # From the AVR
    ack = 0x10
    nack = 0x11


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
            errors.append(f"The checksum didn't match. Expected {expected_crc}")

        return ": ".join(errors)

    @property
    def raw(self) -> bytes:
        return (
            self.start
            + pack("<BB", self.ptype, self.length)
            + self.data
            + pack("<H", self.checksum)
        )


@dataclass
class ResponsePacket(Packet):
    status: int

    @classmethod
    def from_bytes(cls, d: bytes) -> Self:
        inst = super().from_bytes(d)
        inst.status = inst.data[0]
        inst.data = inst.data[1:]
        return inst


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


def packet_size(data_count: int) -> int:
    return 3 + data_count + 2
