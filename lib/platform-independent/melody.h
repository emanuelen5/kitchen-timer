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
