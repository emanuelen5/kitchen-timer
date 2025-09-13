#ifndef LIB_RINGER_H
#define LIB_RINGER_H

#include "stdint.h"
#include "melody.h"

class Ringer
{
public:
    Ringer(void);
    void service();
    void start_melody(MelodyType melody);
    void set_volume(uint8_t volume); // 10 is max, as defined in ToneAC

    static constexpr uint16_t beat_ms = 80;
    static constexpr uint8_t default_volume = 10;

private:
    uint16_t tone_start_time;
    const Note *melody;
    uint8_t note_index;
    uint8_t volume;

    void play_current_note();
    unsigned long get_frequency(Tone note);
};

#endif // LIB_RINGER_H