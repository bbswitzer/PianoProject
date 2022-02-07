#ifndef MIDI_H
#define MIDI_H

#include <Arduino.h>

#define NOTE_OFF_HEADER       8
#define NOTE_ON_HEADER        9
#define CONTROL_CHANGE_HEADER 11
#define SUSTAIN_STATUS_BYTE   64
#define MIN_NOTE_PITCH        21

void checkForMidiUSB();

#endif