import shutil
import textwrap

import pytest
from melody_parser import eval_int_expr, parse_melodies, parse_tone_enum

HAS_CPP = shutil.which("cpp") is not None or shutil.which("gcc") is not None


@pytest.fixture
def tone_map(tmp_path):
    header = textwrap.dedent("""\
        #ifndef LIB_MELODY_H
        #define LIB_MELODY_H
        enum Tone
        {
            NotePause = 0,
            NoteC4 = 262,
            NoteD4 = 294,
            NoteE4 = 330,
            NoteA5 = 880,
            NoteC6 = 1047,
            NoteC8 = 4186,
        };
        #endif
    """)
    p = tmp_path / "melody.h"
    p.write_text(header)
    return parse_tone_enum(p)


class TestParseToneEnum:
    def test_parses_all_tones(self, tone_map):
        assert 0 == tone_map["NotePause"]
        assert 262 == tone_map["NoteC4"]
        assert 4186 == tone_map["NoteC8"]

    def test_ignores_non_enum_lines(self, tmp_path):
        header = textwrap.dedent("""\
            #define SOMETHING 42
            enum Tone
            {
                NoteA5 = 880,
            };
            int x = 999;
        """)
        p = tmp_path / "melody.h"
        p.write_text(header)
        result = parse_tone_enum(p)
        assert {"NoteA5": 880} == result


class TestEvalIntExpr:
    def test_simple_integer(self):
        assert 4 == eval_int_expr("4")

    def test_division(self):
        assert 2 == eval_int_expr("4/2")

    def test_with_spaces(self):
        assert 4 == eval_int_expr(" 8 / 2 ")

    def test_multiplication(self):
        assert 6 == eval_int_expr("3*2")


class TestParseMelodies:
    def test_simple_melody(self, tone_map, tmp_path):
        cpp = textwrap.dedent("""\
            #include "melody.h"

            const Note beep_melody[] = {
                {NoteC8, 8},
                {NotePause, 8},
                {NotePause, 0},
            };
        """)
        p = tmp_path / "melody.cpp"
        p.write_text(cpp)
        melodies = parse_melodies(p, tone_map)
        assert "beep_melody" in melodies
        assert [(4186, 8), (0, 8)] == melodies["beep_melody"]

    def test_multiple_melodies(self, tone_map, tmp_path):
        cpp = textwrap.dedent("""\
            const Note melody_a[] = {
                {NoteC4, 4},
                {NotePause, 0},
            };

            const Note melody_b[] = {
                {NoteD4, 2},
                {NoteE4, 2},
                {NotePause, 0},
            };
        """)
        p = tmp_path / "melody.cpp"
        p.write_text(cpp)
        melodies = parse_melodies(p, tone_map)
        assert 2 == len(melodies)
        assert [(262, 4)] == melodies["melody_a"]
        assert [(294, 2), (330, 2)] == melodies["melody_b"]

    def test_division_in_beats(self, tone_map, tmp_path):
        """Handles cpp-expanded STACATTO output like {NoteC8, 4/2}."""
        cpp = textwrap.dedent("""\
            const Note test_melody[] = {
                {NoteC8, 4/2},
                {NotePause, 4/2},
                {NotePause, 0},
            };
        """)
        p = tmp_path / "melody.cpp"
        p.write_text(cpp)
        melodies = parse_melodies(p, tone_map)
        assert [(4186, 2), (0, 2)] == melodies["test_melody"]

    def test_comments_are_stripped(self, tone_map, tmp_path):
        cpp = textwrap.dedent("""\
            const Note test_melody[] = {
                {NoteA5, 4}, // a comment
                /* block comment */
                {NoteC6, 2},
                {NotePause, 0},
            };
        """)
        p = tmp_path / "melody.cpp"
        p.write_text(cpp)
        melodies = parse_melodies(p, tone_map)
        assert [(880, 4), (1047, 2)] == melodies["test_melody"]

    def test_inline_block_comments(self, tone_map, tmp_path):
        cpp = textwrap.dedent("""\
            const Note test_melody[] = {
                {/*left*/NoteC4, 2},
                {NotePause, 0},
            };
        """)
        p = tmp_path / "melody.cpp"
        p.write_text(cpp)
        melodies = parse_melodies(p, tone_map)
        assert [(262, 2)] == melodies["test_melody"]

    def test_unknown_tone_defaults_to_zero(self, tone_map, tmp_path):
        cpp = textwrap.dedent("""\
            const Note test_melody[] = {
                {NoteUnknown99, 4},
                {NotePause, 0},
            };
        """)
        p = tmp_path / "melody.cpp"
        p.write_text(cpp)
        melodies = parse_melodies(p, tone_map)
        assert [(0, 4)] == melodies["test_melody"]

    def test_empty_melody(self, tone_map, tmp_path):
        cpp = textwrap.dedent("""\
            const Note test_melody[] = {
                {NotePause, 0},
            };
        """)
        p = tmp_path / "melody.cpp"
        p.write_text(cpp)
        melodies = parse_melodies(p, tone_map)
        assert [] == melodies["test_melody"]


@pytest.mark.skipif(not HAS_CPP, reason="C preprocessor not available")
class TestWithPreprocessor:
    @pytest.fixture
    def header_and_tone_map(self, tmp_path):
        header = textwrap.dedent("""\
            #ifndef MELODY_H
            #define MELODY_H
            #include <stdint.h>
            enum Tone {
                NotePause = 0,
                NoteC4 = 262,
                NoteE4 = 330,
                NoteC8 = 4186,
            };
            struct Note {
                enum Tone pitch;
                uint8_t beats;
            };
            constexpr struct Note end_of_melody = {NotePause, 0};
            #endif
        """)
        p = tmp_path / "melody.h"
        p.write_text(header)
        return tmp_path, parse_tone_enum(p)

    def test_macro_expansion_via_cpp(self, header_and_tone_map):
        tmp_path, tone_map = header_and_tone_map
        cpp = textwrap.dedent("""\
            #include "melody.h"
            #define STACATTO(N, D) {N, D/2}, {NotePause, D/2}
            const Note test_melody[] = {
                STACATTO(NoteC8, 4),
                end_of_melody,
            };
        """)
        p = tmp_path / "melody.cpp"
        p.write_text(cpp)
        melodies = parse_melodies(p, tone_map, include_dirs=[tmp_path])
        assert [(4186, 2), (0, 2)] == melodies["test_melody"]

    def test_preprocessor_with_multiple_macros(self, header_and_tone_map):
        tmp_path, tone_map = header_and_tone_map
        cpp = textwrap.dedent("""\
            #include "melody.h"
            #define STACATTO(N, D) {N, D/2}, {NotePause, D/2}
            const Note test_melody[] = {
                STACATTO(NoteC4, 4),
                STACATTO(NoteE4, 2),
                {NoteC8, 8},
                end_of_melody,
            };
        """)
        p = tmp_path / "melody.cpp"
        p.write_text(cpp)
        melodies = parse_melodies(p, tone_map, include_dirs=[tmp_path])
        assert [
            (262, 2),
            (0, 2),
            (330, 1),
            (0, 1),
            (4186, 8),
        ] == melodies["test_melody"]
