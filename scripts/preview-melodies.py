#!/usr/bin/env python3
import argparse
import sys
from pathlib import Path

from melody_parser import find_melody_files, parse_melodies, parse_tone_enum
from pydub import AudioSegment
from pydub.generators import Sine
from pydub.playback import play

PROJECT_ROOT = Path(__file__).resolve().parent.parent


def load_melodies():
    """Load and parse all melodies from the project."""
    header_path = PROJECT_ROOT / "lib" / "platform-independent" / "melody.h"
    if not header_path.exists():
        print(f"Error: melody.h not found at {header_path}", file=sys.stderr)
        sys.exit(1)

    melody_files = find_melody_files(PROJECT_ROOT)
    if not melody_files:
        print("No melody.cpp files found in lib/*/", file=sys.stderr)
        sys.exit(1)

    tone_map = parse_tone_enum(header_path)
    include_dirs = [header_path.parent]

    all_melodies = {}
    for file_path in melody_files:
        all_melodies.update(parse_melodies(file_path, tone_map, include_dirs))

    return all_melodies


def play_melody(notes, beat_ms=150):
    """Play a melody from list of (frequency, beats) tuples."""
    melody = AudioSegment.empty()

    for frequency, beats in notes:
        duration = beats * beat_ms
        if frequency > 0:
            tone = Sine(frequency).to_audio_segment(duration=duration)
        else:
            tone = AudioSegment.silent(duration=duration)
        melody += tone

    play(melody)


def main():
    all_melodies = load_melodies()
    melody_names = list(all_melodies.keys())

    parser = argparse.ArgumentParser(description="Preview kitchen timer melodies")
    parser.add_argument(
        "melody",
        choices=melody_names,
        help="Name of the melody to play",
    )
    args = parser.parse_args()

    print(f"Playing {args.melody}...")
    play_melody(all_melodies[args.melody])


if __name__ == "__main__":
    main()
