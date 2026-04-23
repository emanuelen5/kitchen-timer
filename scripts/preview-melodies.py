#!/usr/bin/env python3
import argparse
import sys
from pathlib import Path

from melody_parser import find_melody_files, parse_melodies, parse_tone_enum
from pydub import AudioSegment
from pydub.generators import Sine, Square
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


def make_tick(beat_ms):
    """Create a short high-pitched tick sound for one beat."""
    tick_duration = min(15, beat_ms)
    tick = Sine(1000).to_audio_segment(duration=tick_duration) - 25
    return tick + AudioSegment.silent(duration=beat_ms - tick_duration)


def play_melody(notes, beat_ms=150, tick: None | int = None):
    """Play a melody from list of (frequency, beats) tuples."""
    melody = AudioSegment.empty()

    for frequency, beats in notes:
        duration = beats * beat_ms
        if frequency > 0:
            tone = Square(frequency).to_audio_segment(duration=duration) - 40
        else:
            tone = AudioSegment.silent(duration=duration)
        melody += tone

    if tick is not None:
        one_tick = make_tick(beat_ms)
        tick_track = AudioSegment.empty()
        total_beats = sum(b for _, b in notes)
        for beat in range(0, int(total_beats), tick):
            position = beat * beat_ms
            padding = position - len(tick_track)
            if padding > 0:
                tick_track += AudioSegment.silent(duration=padding)
            tick_track += one_tick
        melody = melody.overlay(tick_track)

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
    parser.add_argument(
        "--bpm",
        type=int,
        default=100,
        help="Tempo in beats per minute (default: 100)",
    )
    parser.add_argument(
        "--start",
        type=int,
        default=0,
        help="Beat position to start playback from (default: 0)",
    )
    parser.add_argument(
        "--beats",
        type=int,
        default=None,
        help="Number of beats to play from the start position",
    )
    parser.add_argument(
        "--tick",
        type=int,
        help="Overlay a metronome tick on each N beats",
    )
    args = parser.parse_args()

    if args.tick is not None and args.tick <= 0:
        parser.error("--tick must be a non-negative integer")

    notes = all_melodies[args.melody]

    # Slice by beat position
    if args.start > 0 or args.beats is not None:
        beat_pos = 0
        start_idx = len(notes)
        end_idx = len(notes)
        for i, (_, b) in enumerate(notes):
            if beat_pos < args.start:
                beat_pos += b
                continue
            if beat_pos == args.start and start_idx == len(notes):
                start_idx = i
            beat_pos += b
            if args.beats is not None and beat_pos - args.start >= args.beats:
                end_idx = i + 1
                break
        notes = notes[start_idx:end_idx]

    # 4/4 th time signature
    beat_ms = 60_000 / 4 // args.bpm
    end_beat = args.start + args.beats if args.beats is not None else None
    range_str = (
        f" (beats {args.start}:{end_beat})"
        if args.start > 0 or args.beats is not None
        else ""
    )
    print(f"Playing {args.melody} at {args.bpm} BPM{range_str}...")
    try:
        play_melody(notes, beat_ms=beat_ms, tick=args.tick)
    except KeyboardInterrupt:
        print("\nPlayback stopped.")


if __name__ == "__main__":
    main()
