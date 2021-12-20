#include "midi_handler.h"
#include "mos.h"
#include "multivoices.h"
#include "menu.h"

extern struct midi_info_display_t midi_info_display;
extern Menu *menu;

// TODO: handle MultiVoices? (sets all voices to the same parameters)
int env0attack, env0decay, env0sustain, env0release;
int filter0freq, filter0resonance;

class MultiVoices;
extern MultiVoices *voices;

#define CC_VOICE1_WAVE 17
#define CC_VOICE1_ATTACK 16
#define CC_VOICE1_DECAY 15
#define CC_VOICE1_SUSTAIN 14

void handleMidiControlChange(byte channel, byte control, byte value)
{
    midi_info_display.note = 0;
    midi_info_display.control = control;
    midi_info_display.value = value;
    if (!midi_info_display.updated) {
        midi_info_display.last_updated = millis();
        midi_info_display.updated = true;
    }

    Serial.print("CC ");
    Serial.print(control);
    Serial.print(" ");
    Serial.println(value);

    switch (control) {
    case 1:
        voices->setPwm(value << 4);
        Serial.print("Set PWM ");
        Serial.println(value << 4);
        break;
    case 2:
        //voice_enabled[0] = value == 0 ? false : true;
        filter0freq = value << 4;
        //sid_synth->setFrequency(0, filter0freq);
        Serial.print("Set filter frequency ");
        Serial.println(filter0freq);
        break;
    case 3:
        //voice_enabled[1] = value == 0 ? false : true;
        filter0resonance = value << 4;
        //sid_synth->setFrequency(0, filter0resonance);
        Serial.print("Set filter resonance ");
        Serial.println(filter0resonance);
        break;
    case 4:
        voices->arp_enabled = !voices->arp_enabled;
        Serial.print("Arpegiator ");
        Serial.println(voices->arp_enabled);
    case 5:
        //env0attack = value;
        //sid_synth->setADEnvelope(0, env0attack, env0decay);
        if (value < 5) value = 0; // limit minimum arp interval to 5*2=10 ms
        voices->arp_interval = value << 1;
        voices->arp_enabled = voices->arp_interval > 0;
        Serial.print("Set arp interval ");
        Serial.println(voices->arp_interval);
        break;
    case 6:
        env0decay = value;
        //sid_synth->setADEnvelope(0, env0attack, env0decay);
        Serial.print("Set decay ");
        Serial.println(env0decay);
        break;
    case 7:
        env0sustain = value;
        //sid_synth->setSREnvelope(0, env0sustain, env0release);
        Serial.print("Set sustain ");
        Serial.println(env0sustain);
        break;
    case 8:
        env0release = value;
        //sid_synth->setSREnvelope(0, env0sustain, env0release);
        Serial.print("Set release ");
        Serial.println(env0release);
        break;
    case 9: // encoder
        if (value == 64) {
            menu->rotate_left();
        } else if (value == 127) {
            menu->rotate_right();
        }
        break;
    case 18: // encoder button
        if (value == 127) menu->select();
        break;
    case 10: // red button
        voices->arp_enabled = !voices->arp_enabled;
        Serial.print("Arpegiator ");
        Serial.println(voices->arp_enabled);
        break;
    }
}

void handleMidiNoteOn(byte channel, byte note, byte vel)
{
    midi_info_display.control = 0;
    midi_info_display.note = note;
    midi_info_display.value = vel;
    if (!midi_info_display.updated) {
        midi_info_display.last_updated = millis();
        midi_info_display.updated = true;
    }

    Serial.print("Note On: ");
    Serial.println(note);
    voices->playNote(note);
}

void handleMidiNoteOff(byte channel, byte note, byte vel)
{
    Serial.print("Note Off: ");
    Serial.println(note);
    voices->stopNote(note);
}
