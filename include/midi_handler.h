#ifndef __MIDI_HANDLER_H__
#define __MIDI_HANDLER_H__

#include <Arduino.h>

struct midi_info_display_t {
    uint8_t control;
    uint8_t note;
    uint8_t value;
    unsigned long last_updated;
    bool updated;
};

void handleMidiControlChange(byte channel, byte control, byte value);
void handleMidiNoteOn(byte channel, byte note, byte vel);
void handleMidiNoteOff(byte channel, byte note, byte vel);

#endif