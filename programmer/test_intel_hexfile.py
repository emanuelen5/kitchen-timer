from intelhex import IntelHex

from programmer.intel_hexfile import (
    PageData,
    address_to_page,
    empty_byte,
    get_all_pagedata,
    get_data_for_page,
    get_pages_to_write,
    page_size,
)


def test_first_page():
    assert address_to_page(0) == 0
    assert address_to_page(63) == 0
    assert address_to_page(64) == 1


def test_get_data_for_page():
    byte = b"\xaa"

    i = IntelHex()
    i.puts(0, byte)

    assert get_data_for_page(i, 0) == byte + empty_byte * (page_size - 1)


def test_get_pages_to_write():
    i = IntelHex()
    i.puts(page_size, b"\x00")

    assert get_pages_to_write(i) == {1}


def test_get_all_pagedata():
    i = IntelHex()
    i.puts(page_size, b"\x00")
    assert get_all_pagedata(i) == [PageData(1, b"\x00" + (b"\xff" * (page_size - 1)))]
