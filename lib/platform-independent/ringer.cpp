#include "ringer.h"
#include "toneAC.h"
#include "melody.h"

// Included here to avoid linker issues on desktop builds
uint16_t millis(void);

Ringer::Ringer() : tone_start_time(0),
                   melody(nullptr),
                   note_index(0),
                   volume(10)
{
}

void Ringer::start_melody(MelodyType melody)
{
    this->tone_start_time = millis();
    this->note_index = 0;
    this->melody = get_melody(melody);
}

void Ringer::service()
{
    if (melody == nullptr)
        return;

    Note note = melody[this->note_index];
    if (is_end_of_melody(&note))
    {
        return;
    }

    uint16_t current_time = millis();
    if (current_time - this->tone_start_time >= (beat_ms * note.beats))
    {
        this->tone_start_time = current_time;
        this->note_index++;
        play_current_note();
    }
}

void Ringer::play_current_note()
{
    Note note = melody[this->note_index];
    if (note.pitch == NotePause)
    {
        noToneAC();
    }
    else
    {
        toneAC(note.pitch, this->volume, PLAY_FOREVER, true);
    }
}

void Ringer::set_volume(uint8_t volume)
{
    if (volume > 10)
    {
        volume = 10;
    }
    this->volume = volume;
}
