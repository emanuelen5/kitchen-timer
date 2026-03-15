# Buzzer Environment

## Purpose

Tests the passive buzzer hardware by playing a melody at progressively increasing volume levels.

## Hardware Requirements

- Passive buzzer connected to the buzzer output pin

## Expected Outcome

The buzzer plays "Für Elise" at progressively increasing volume levels. Each time the melody finishes, the volume is raised and the melody restarts, until the default (maximum) volume is reached. After the final playback the buzzer stops.
