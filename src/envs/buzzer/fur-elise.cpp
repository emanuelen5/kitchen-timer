#include "toneAC.h"

#define SIXTEENTH_DURATION 60

// Enum defining musical notes
enum Notes
{
    NotePause = 0,
    NoteA3,
    NoteAS3,
    NoteB3,
    NoteC4,
    NoteCS4,
    NoteD4,
    NoteDS4,
    NoteE4,
    NoteF4,
    NoteFS4,
    NoteG4,
    NoteGS4,
    NoteA4,
    NoteAS4,
    NoteB4,
    NoteC5,
    NoteCS5,
    NoteD5,
    NoteDS5,
    NoteE5,
    NoteF5,
    NoteFS5,
    NoteG5,
    NoteGS5,
    NoteA5,
    NoteAS5,
    NoteB5,
    NoteSentinel
};

// Define struct for Note
struct Note
{
    enum Notes pitch;
    int beats; // Duration in beats
};

// Function to play a tone with specified duration
void playTone(int pitch, int beats)
{
    toneAC(pitch, 10, beats * SIXTEENTH_DURATION);
}

// Function to add a pause with specified duration
void addPause(int beats)
{
    delay(beats * SIXTEENTH_DURATION);
}

// Define an array to map notes to their frequencies
const int noteFrequencies[] = {
    // https://www.songstuff.com/recording/article/music_note_fundamental_frequencies/
    0,   // Pause
    220, // A3
    233, // AS3
    247, // B3
    262, // C4
    277, // CS4
    294, // D4
    311, // DS4
    330, // E4
    349, // F4
    370, // FS4
    392, // G4
    415, // GS4
    440, // A4
    466, // AS4
    494, // B4
    523, // C5
    554, // CS5
    587, // D5
    622, // DS5
    659, // E5
    698, // F5
    740, // FS5
    784, // G5
    831, // GS5
    880, // A5
    932, // AS5
    988, // B5
};

unsigned long get_frequency(enum Notes note)
{
    if (note <= NotePause || note >= NoteSentinel)
    {
        return 0;
    }
    return noteFrequencies[note];
}

void playMelody(Note melody[], int size)
{
    // Play the melody
    for (int i = 0; i < size; i++)
    {
        if (melody[i].pitch == NotePause)
        {
            // Pause
            addPause(melody[i].beats);
        }
        else
        {
            unsigned long freq = get_frequency(melody[i].pitch);
            playTone(freq, melody[i].beats);
        }
    }
}

#define ALEN(array) (sizeof(array) / sizeof(array[0]))

void play_fur_elise()
{
    // Define the melody notes and durations
    Note melody[] = {
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
    };

    int melodySize = ALEN(melody);
    // Play the melody
    playMelody(melody, melodySize);
}
