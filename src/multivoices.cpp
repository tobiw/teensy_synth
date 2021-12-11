#include "multivoices.h"
#include "note_freqs.h"
#include <Audio.h>

AudioPlaySID playSID[NUM_VOICES];
AudioMixer4 mixer[3];
AudioOutputI2S i2s;
AudioConnection patchCord1(playSID[4], 0, mixer[1], 0);
AudioConnection patchCord2(playSID[2], 0, mixer[0], 2);
AudioConnection patchCord3(playSID[1], 0, mixer[0], 1);
AudioConnection patchCord4(playSID[0], 0, mixer[0], 0);
AudioConnection patchCord5(playSID[3], 0, mixer[0], 3);
AudioConnection patchCord6(playSID[5], 0, mixer[1], 1);
AudioConnection patchCord7(playSID[7], 0, mixer[1], 3);
AudioConnection patchCord8(playSID[6], 0, mixer[1], 2);
AudioConnection patchCord9(mixer[1], 0, mixer[2], 1);
AudioConnection patchCord10(mixer[0], 0, mixer[2], 0);
AudioConnection patchCord11(mixer[2], 0, i2s, 0);
AudioConnection patchCord12(mixer[2], 0, i2s, 1);


MultiVoices::MultiVoices()
{
    arp_enabled = false;
    current_note = 1;

    memset(voiceActive, 0, sizeof (bool) * NUM_VOICES);
    memset(notes, -1, sizeof (int) * 127);

    for (int i = 0; i < NUM_VOICES; i++)
    {
        MOS6581 *p = new MOS6581(&playSID[i]);
        p->volume(8);
        p->setMode(0, SID_SQUARE);
        p->setPulseWidth(0, 2048);
        p->setVoice(0, true);
        p->setADEnvelope(0, 5, 0);
        p->setSREnvelope(0, 15, 0);
        p->setFilter(0, false);
        p->setMode(1, SID_TRIANGLE);
        p->setVoice(1, true);
        p->setADEnvelope(1, 3, 0);
        p->setSREnvelope(1, 15, 0);
        p->setFilter(1, false);
        sidChips[i] = p;
    }

    for (int i = 0; i < 4; i++)
    {
        // 1/8th gain for each voice
        mixer[0].gain(i, 0.125);
        mixer[1].gain(i, 0.125);
    }
}

void MultiVoices::setPwm(int pwm)
{
    for (int i = 0; i < NUM_VOICES; i++)
    {
        sidChips[i]->setPulseWidth(0, pwm);
        sidChips[i]->setPulseWidth(1, pwm);
    }
}

void MultiVoices::updateArpegiator()
{
    if (arp_enabled && voiceActive[0])
    {
        // 1-> 4 -> 3 (major)
        if (current_note == 1) current_note += 4;
        else if (current_note == 5) current_note += 3;
        else current_note = 1;

        sidChips[0]->setFrequency(0, note_to_freq_map[arp_root_note + current_note] << 2);
        sidChips[0]->setFrequency(1, note_to_freq_map[arp_root_note + current_note] << 4);
    }
}

void MultiVoices::playNote(int note)
{
    if (notes[note] == -1)
    {
        const int v = getNextVoice();
        notes[note] = v;
        arp_root_note = note;

        Serial.print("Playing note ");
        Serial.print(note);
        Serial.print(" with voice ");
        Serial.println(v);

        sidChips[v]->setVoice(0, true);
        sidChips[v]->setVoice(1, true);
        sidChips[v]->setFrequency(0, note_to_freq_map[note] << 2);
        sidChips[v]->setFrequency(1, note_to_freq_map[note] << 4);
    }
}

void MultiVoices::stopNote(int note)
{
    const int v = notes[note];
    if (v >= 0)
    {
        freeupVoice(v);
        notes[note] = -1;

        Serial.print("Stopped voice ");
        Serial.print(v);
        Serial.print(" (note ");
        Serial.print(note);
        Serial.println("(");

        sidChips[v]->setVoice(0, false);
        sidChips[v]->setVoice(1, false);
        // TODO: handle release?
    }
}

int MultiVoices::getNextVoice()
{
    Serial.println("MultiVoices::getNextVoice");
    for (int i = 0; i < NUM_VOICES; i++)
    {
        if (voiceActive[i] == false)
        {
            voiceActive[i] = true;
            return i;
        }
    }
    return -1; // no free voice found
}

void MultiVoices::freeupVoice(int i)
{
    Serial.println("MultiVoices::freeupVoice");
    if (i < NUM_VOICES)
    {
        voiceActive[i] = false;
    }
}
