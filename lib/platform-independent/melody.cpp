#include "melody.h"

const Note beep_melody[] = {
    {NoteC8, 8},
    {NotePause, 8},
    end_of_melody,
};

const Note ten_beeps_melody[] = {
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

const Note volume_setting_melody[] = {
    {NoteA6, 1},
    end_of_melody,
};

const Note saw_melody[] = {
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

const Note fur_elise_melody[] = {
    {NoteE4, 4},
    {NoteDS4, 4},
    {NoteE4, 4},
    {NoteDS4, 4},
    {NoteE4, 4},
    {NoteB3, 4},
    {NoteD4, 4},
    {NoteC4, 4},
    {NoteA3, 8},
    {NotePause, 4},
    {NoteC4, 4},
    {NoteE4, 4},
    {NoteA4, 4},
    {NoteB4, 8},
    {NotePause, 4},
    {NoteE4, 4},
    {NoteGS4, 4},
    {NoteB4, 4},
    {NoteC5, 8},
    {NotePause, 16},
    end_of_melody,
};

const Note *melodies[] = {
    beep_melody,
    ten_beeps_melody,
    volume_setting_melody,
    saw_melody,
    fur_elise_melody,
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
