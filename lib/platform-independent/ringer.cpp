#include "ringer.h"
#include "melody.h"

// Included here to avoid linker issues on desktop builds
void noToneAC();
void toneAC(unsigned long, uint8_t, unsigned long, uint8_t);
uint16_t millis(void);

Ringer::Ringer() : tone_start_time(0),
                   melody(nullptr),
                   note_index(0),
                   volume(10)
{
}

void Ringer::start_melody(MelodyType melody, uint8_t repeats)
{
    this->tone_start_time = millis();
    this->note_index = 0;
    this->melody = get_melody(melody);
    this->repeats = repeats;
}

void Ringer::service(void)
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

void Ringer::play_current_note(void)
{
    Note note = melody[this->note_index];
    if (is_end_of_melody(&note) && repeats > 0)
    {
        repeats--;
        this->note_index = 0;
    }
    else if (note.pitch == NotePause)
    {
        noToneAC();
    }
    else
    {
        const unsigned long play_forever = 0;
        toneAC(note.pitch, this->volume, play_forever, true);
    }
}

void Ringer::stop(void)
{
    this->melody = nullptr;
}

void Ringer::set_volume(uint8_t volume)
{
    if (volume > 10)
    {
        volume = 10;
    }
    this->volume = volume;
}
