#include "buzzer.h"
#include "melody.h"

// Included here to avoid linker issues on desktop builds
void noToneAC();
void toneAC(unsigned long, uint8_t);
uint16_t millis(void);

Buzzer::Buzzer() : tone_start_time(0),
                   melody(nullptr),
                   note_index(0),
                   volume(10)
{
}

void Buzzer::start_melody(MelodyType melody, uint8_t repeats)
{
    this->start_melody(get_melody(melody), repeats);
}

void Buzzer::start_melody(const Note *melody, uint8_t repeats)
{
    this->tone_start_time = millis();
    this->note_index = 0;
    this->melody = melody;
    this->repeats = repeats;
    play_current_note();
}

void Buzzer::service(void)
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

void Buzzer::play_current_note(void)
{
    Note note = melody[this->note_index];
    if (is_end_of_melody(&note) && repeats > 0)
    {
        this->start_melody(this->melody, this->repeats - 1);
    }
    else if (note.pitch == NotePause)
    {
        noToneAC();
    }
    else
    {
        toneAC(note.pitch, this->volume);
    }
}

void Buzzer::stop(void)
{
    this->melody = nullptr;
    noToneAC();
}

void Buzzer::set_volume(uint8_t volume)
{
    if (volume > 10)
    {
        volume = 10;
    }
    this->volume = volume;
}

bool Buzzer::is_finished(void)
{
    if (this->melody == nullptr)
        return true;

    if (this->repeats != 0)
        return false;

    return is_end_of_melody(&this->melody[this->note_index]);
}

uint8_t Buzzer::get_volume(void)
{
    return this->volume;
}
