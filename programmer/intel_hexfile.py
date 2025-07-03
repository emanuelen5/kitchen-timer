from dataclasses import dataclass
from pathlib import Path

from intelhex import IntelHex

page_size = 128
empty_byte = b"\xff"


@dataclass
class PageData:
    offset: int
    data: bytes

    def __post_init__(self):
        assert len(self.data) == page_size


def address_to_page(addr: int) -> int:
    return addr >> 7


def get_data_for_page(ih: IntelHex, page: int) -> bytes:
    return bytes([ih[a] for a in range(page * page_size, (page + 1) * page_size)])


def get_pages_to_write(ih: IntelHex) -> set[int]:
    return set(address_to_page(a) for a in ih.addresses())


def get_all_pagedata(ih: IntelHex) -> list[PageData]:
    return [PageData(p, get_data_for_page(ih, p)) for p in get_pages_to_write(ih)]


def read_all_pagedata(filename: Path) -> list[PageData]:
    ih = IntelHex()
    ih.loadhex(filename)
    return get_all_pagedata(ih)


def write_all_pagedata(filename: Path, pagedata: list[PageData]) -> None:
    ih = IntelHex()
    for page in pagedata:
        for i in range(page_size):
            ih[page.offset * page_size + i] = page.data[i]
    ih.write_hex_file(filename)
