#include "melody.h"

#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif

PROGMEM const Note beep_melody[] = {
    {NoteC8, 8},
    {NotePause, 8},
    end_of_melody,
};

PROGMEM const Note ten_beeps_melody[] = {
    {NoteC8, 8},
    {NotePause, 8},
    {NoteC8, 8},
    {NotePause, 8},
    {NoteC8, 8},
    {NotePause, 8},
    {NoteC8, 8},
    {NotePause, 8},
    {NoteC8, 8},
    {NotePause, 8},
    {NoteC8, 8},
    {NotePause, 8},
    {NoteC8, 8},
    {NotePause, 8},
    {NoteC8, 8},
    {NotePause, 8},
    {NoteC8, 8},
    {NotePause, 8},
    {NoteC8, 8},
    {NotePause, 8},
    end_of_melody,
};

PROGMEM const Note volume_setting_melody[] = {
    {NoteA6, 1},
    end_of_melody,
};

PROGMEM const Note saw_melody[] = {
    {NoteC7, 1},
    {NoteD7, 1},
    {NoteE7, 1},
    {NoteF7, 1},
    {NoteG7, 1},
    {NoteA7, 1},
    {NoteB7, 1},
    {NoteC8, 1},
    {NoteB7, 1},
    {NoteA7, 1},
    {NoteG7, 1},
    {NoteF7, 1},
    {NoteE7, 1},
    {NoteD7, 1},
    end_of_melody,
};

PROGMEM const Note fur_elise_melody[] = {
    {NoteE6, 4},
    {NoteDS6, 4},
    {NoteE6, 4},
    {NoteDS6, 4},
    {NoteE6, 4},
    {NoteB5, 4},
    {NoteD6, 4},
    {NoteC6, 4},
    {NoteA5, 8},
    {NotePause, 4},
    {NoteC6, 4},
    {NoteE6, 4},
    {NoteA6, 4},
    {NoteB6, 8},
    {NotePause, 4},
    {NoteE6, 4},
    {NoteGS6, 4},
    {NoteB6, 5},
    {NoteC7, 8},
    {NotePause, 16},
    end_of_melody,
};

// Stacatto
#define STACATTO(N, D) {N, D/2}, {NotePause, D/2}

PROGMEM const Note super_mario_melody[] = {
    // Bar 1-2
    STACATTO(NoteE6, 2),
    STACATTO(NoteE6, 2),
    {NotePause, 2},
    STACATTO(NoteE6, 2),
    {NotePause, 2},
    {NoteC6, 2},
    {NoteE6, 4},
    {NoteG6, 4}, {NotePause, 4},
    {NoteG5, 4}, {NotePause, 4},
    // Bar 3
    {NoteC6, 4}, {NotePause, 2},
    {NoteG5, 4}, {NotePause, 2},
    {NoteE5, 4},
    // Bar 4
    {NotePause, 2},
    {NoteA5, 4}, {NoteB5, 4}, {NoteAS5, 2}, {NoteA5, 4},
    // Bar 5
    {NoteG5, 2}, {NoteE6, 2}, {NotePause, 2}, {NoteG6, 2},
    {NoteA6, 2}, {NotePause, 2}, {NoteF6, 2}, {NoteG6, 2},
    // Bar 6
    {NotePause, 2}, {NoteE6, 2}, {NotePause, 2}, {NoteC6, 2},
    {NoteD6, 2}, {NoteB5, 2}, {NotePause, 4},
    // Bar 7, repeat of bar 3
    {NoteC6, 4}, {NotePause, 2},
    {NoteG5, 4}, {NotePause, 2},
    {NoteE5, 4},
    // // Bar 8
    {NotePause, 2},
    {NoteA5, 4}, {NoteB5, 4}, {NoteAS5, 2}, {NoteA5, 4},
    // // Bar 9
    {NoteG5, 2}, {NoteE6, 2}, {NotePause, 2}, {NoteG6, 2},
    {NoteA6, 2}, {NotePause, 2}, {NoteF6, 2}, {NoteG6, 2},
    // Bar 10
    {NotePause, 2}, {NoteE6, 2}, {NotePause, 2}, {NoteC6, 2},
    {NoteD6, 2}, {NoteB5, 2}, {NotePause, 4},
    // Measure 11-12: Part B - descending run
    {/*left*/NoteC4, 4}, {NoteG6, 2}, {NoteFS6, 2}, {NoteF6, 2}, {NoteDS6, 2}, {/*left*/NoteC5, 2}, {NoteE6, 2},
    // Bar 12
    {/*left*/NoteF4, 2}, {NoteGS5, 2}, {NoteA5, 2}, {NoteC6, 2}, {/*left*/NoteC5, 2}, {NoteA5, 2}, {/*left*/NoteF4, 2}, {NoteD6, 2},
    // Bar 13-14
    {/*left*/NoteC4, 4}, {NoteG6, 2}, {NoteFS6, 2}, {NoteF6, 2}, {NoteDS6, 2}, {/*left*/NoteG4, 2},
    {NoteE6, 4}, 
    {NoteC7, 2}, {NotePause, 2}, STACATTO(NoteC7, 2), {NoteC7, 4}, {NoteG4, 4}, 
    // Measure 15-16: Part B repeat
    {/*left*/NoteC4, 4}, {NoteG6, 2}, {NoteFS6, 2}, {NoteF6, 2}, {NoteDS6, 2}, {/*left*/NoteC5, 2}, {NoteE6, 2},
    {/*left*/NoteF4, 2}, {NoteGS5, 2}, {NoteA5, 2}, {NoteC6, 2}, {/*left*/NoteF4, 2}, {NoteA5, 2}, {/*left*/NoteC5, 2}, {NoteD6, 2},
    // Measure 17-18
    {NoteC4, 4}, {NoteDS5, 2}, {NoteGS4, 2}, {NotePause, 2}, {NoteD5, 2}, {NoteAS4, 2},
    {NotePause, 2}, {NoteC5, 6}, STACATTO(NoteG4, 2), {NoteG4, 4}, {NoteC4, 4},
    end_of_melody,
};

const Note *melodies[] = {
    beep_melody,
    ten_beeps_melody,
    volume_setting_melody,
    saw_melody,
    fur_elise_melody,
    super_mario_melody,
};

const Note *get_melody(MelodyType melody)
{
    if (melody >= melody_count)
    {
        return melodies[0]; // default to beep melody if invalid melody type
    }
    return melodies[melody];
}

bool is_end_of_melody(const Note *note)
{
    return note->pitch == NotePause && note->beats == 0;
}
