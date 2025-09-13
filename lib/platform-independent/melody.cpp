#include "melody.h"

const Note beeps_melody[] = {
    {NoteC5, 8},
    {NotePause, 8},
    end_of_melody,
};

const Note saw_melody[] = {
    {NoteC4, 1},
    {NoteD4, 1},
    {NoteE4, 1},
    {NoteF4, 1},
    {NoteG4, 1},
    {NoteA4, 1},
    {NoteB4, 1},
    {NoteC5, 1},
    {NoteB4, 1},
    {NoteA4, 1},
    {NoteG4, 1},
    {NoteF4, 1},
    {NoteE4, 1},
    {NoteD4, 1},
    end_of_melody,
};

const Note *get_melody(MelodyType melody)
{
    switch (melody)
    {
    case beeps:
        return beeps_melody;
    case saw:
        return saw_melody;
    default:
        return nullptr;
    }
}

bool is_end_of_melody(const Note *note)
{
    return note->pitch == NotePause && note->beats == 0;
}
