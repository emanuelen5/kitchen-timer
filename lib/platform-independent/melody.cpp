#include "melody.h"

const Note beeps_melody[] = {
    {NoteC8, 8},
    {NotePause, 8},
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
