#ifndef LIB_RINGER_H
#define LIB_RINGER_H

#include "stdint.h"
#include "melody.h"

class Buzzer
{
public:
    Buzzer(void);
    void service(void);
    void start_melody(MelodyType melody, uint8_t repeats);
    void start_melody(const Note *melody, uint8_t repeats);
    void set_volume(uint8_t volume); // 10 is max, as defined in ToneAC
    void stop(void);
    bool is_finished(void);

    static constexpr uint16_t beat_ms = 80;
    static constexpr uint8_t default_volume = 10;

private:
    uint16_t tone_start_time;
    const Note *melody;
    uint8_t note_index;
    uint8_t volume;
    uint8_t repeats;

    void play_current_note(void);
    unsigned long get_frequency(Tone note);
};

#endif // LIB_RINGER_H