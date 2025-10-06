#ifndef LIB_MELODY_H
#define LIB_MELODY_H

#include <stdint.h>

enum MelodyType
{
    beeps,
    saw,
};

enum Tone
{
    NotePause = 0,
    NoteA3 = 220,
    NoteAS3 = 233,
    NoteB3 = 247,
    NoteC4 = 262,
    NoteCS4 = 277,
    NoteD4 = 294,
    NoteDS4 = 311,
    NoteE4 = 330,
    NoteF4 = 349,
    NoteFS4 = 370,
    NoteG4 = 392,
    NoteGS4 = 415,
    NoteA4 = 440,
    NoteAS4 = 466,
    NoteB4 = 494,
    NoteC5 = 523,
    NoteCS5 = 554,
    NoteD5 = 587,
    NoteDS5 = 622,
    NoteE5 = 659,
    NoteF5 = 698,
    NoteFS5 = 740,
    NoteG5 = 784,
    NoteGS5 = 831,
    NoteA5 = 880,
    NoteAS5 = 932,
    NoteB5 = 988,
    NoteC6 = 1047,
    NoteCS6 = 1109,
    NoteD6 = 1175,
    NoteDS6 = 1245,
    NoteE6 = 1319,
    NoteF6 = 1397,
    NoteFS6 = 1480,
    NoteG6 = 1568,
    NoteGS6 = 1661,
    NoteA6 = 1760,
    NoteAS6 = 1865,
    NoteB6 = 1976,
    NoteC7 = 2093,
    NoteCS7 = 2217,
    NoteD7 = 2349,
    NoteDS7 = 2489,
    NoteE7 = 2637,
    NoteF7 = 2794,
    NoteFS7 = 2960,
    NoteG7 = 3136,
    NoteGS7 = 3322,
    NoteA7 = 3520,
    NoteAS7 = 3729,
    NoteB7 = 3951,
    NoteC8 = 4186,
    NoteCS8 = 4435,
    NoteD8 = 4699,
    NoteDS8 = 4978,
    NoteE8 = 5274,
    NoteF8 = 5588,
    NoteFS8 = 5920,
    NoteG8 = 6272,
    NoteGS8 = 6645,
    NoteA8 = 7040,
    NoteAS8 = 7459,
    NoteB8 = 7902,
    NoteC9 = 8372,
};

struct Note
{
    enum Tone pitch;
    uint8_t beats;
};

constexpr struct Note end_of_melody = {NotePause, 0};

const Note* get_melody(MelodyType melody);
bool is_end_of_melody(const Note* note);

#endif // LIB_MELODY_H
