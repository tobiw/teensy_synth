#ifndef __MULTIVOICES_H__
#define __MULTIVOICES_H__

#include "mos.h"

#define NUM_VOICES 8

/**
 * @brief Manages multiple virtual SID chips (via Teensy Audio patching) to enable polyphony.
 * 
 */
class MultiVoices
{
public:
    MultiVoices();
    int getNextVoice();
    void freeupVoice(int i); // not on NoteOff but when note has rung out (after release period)
    void playNote(int note);
    void stopNote(int note);
    void updateArpegiator();
    void setPwm(int pwm);

    bool arp_enabled;

protected:
    bool voiceActive[NUM_VOICES];
    int notes[127];
    MOS6581 *sidChips[NUM_VOICES];

    // Arpegiator
    int current_note;
    int arp_root_note;
};

#endif
