#ifndef __MOS_H_
#define __MOS_H_

#include "reSID.h"
#include "sid_registers.h"

#define SID_NOISE 128
#define SID_SQUARE 64
#define SID_RAMP 32
#define SID_TRIANGLE 16
#define SID_TEST 8
#define SID_RING 20
#define SID_SYNC 66
#define SID_OFF 0

#define SID_3OFF 128
#define SID_FILT_HP 64
#define SID_FILT_BP 32
#define SID_FILT_LP 16
#define SID_FILT_OFF 0

#define SID_FILT_VOICE1 1
#define SID_FILT_VOICE2 2
#define SID_FILT_VOICE3 4
#define SID_FILT_EXT 8

class AudioPlaySID;

class MOS6581
{
  // create some internal registers for binary manipulation
  byte voice1_register;
  byte voice2_register;
  byte voice3_register;
  byte filter_register;
  byte mode_register;

  void voiceFrequency(byte low, byte high, word frequency);
  void voicePulseWidth(byte low, byte high, word frequency);
  void _setFilterOnOff(byte voice, byte *reg, boolean on);
  void _setVoiceOnOff(byte *reg, boolean on);

  AudioPlaySID *playSID;

public:
  MOS6581(AudioPlaySID *p);
  void reset(void);
  void transfer(byte address, byte value);

  void setVoice(byte voice, boolean on);
  void setFilter(byte voice, boolean on);

  // fundamental frequency of waveform generator. 16bit number
  void setFrequency(byte voice, word frequency);

  // duty cycle of square waves. 12bit number
  void setPulseWidth(byte voice, word frequency);

  void setMode(byte voice, byte mode);

  void setADEnvelope(byte voice, byte attack, byte decay);
  void setSREnvelope(byte voice, byte sustain, byte release);

  // filter volume and output
  void volume(byte value);

  void setFilterMode(byte mode);

  // set the filter frequency. 11bit number
  void filterFrequency(word frequency);
  void filterResonance(byte resonance);
};

#endif
