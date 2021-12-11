#include <mos.h>

MOS6581::MOS6581(AudioPlaySID *p) {
  voice1_register = B00100000;
  voice2_register = B00100000;
  voice3_register = B00100000;
  filter_register = B00000000;
  mode_register = B00000000;

  playSID = p;
}

void MOS6581::reset(){
  // iterate through all the registers and reset them
  for(int i = 0; i < 25; i++){
    transfer(i, 0);
  }
}

void MOS6581::transfer(byte address, byte value){
  playSID->setreg(address, value);
}

void MOS6581::voiceFrequency(byte lowAddress, byte highAddress, word frequency) {
  transfer(lowAddress, lowByte(frequency));
  transfer(highAddress, highByte(frequency));

}

void MOS6581::voicePulseWidth(byte lowAddress, byte highAddress, word frequency) {
  transfer(lowAddress, lowByte(frequency));
  transfer(highAddress, highByte(frequency) & B00001111);
}

void MOS6581::setFrequency(byte voice, word frequency){
  byte low = 0, high = 0;
  if (voice == 0) {
    low = SID_V1_FL;
    high = SID_V1_FH;
  } else if (voice == 1) {
    low = SID_V2_FL;
    high = SID_V2_FH;
  } else if (voice == 2) {
    low = SID_V3_FL;
    high = SID_V3_FH;
  }
  voiceFrequency(low, high, frequency);
}

void MOS6581::setPulseWidth(byte voice, word frequency) {
  byte low = 0, high = 0;
  if (voice == 0) {
    low = SID_V1_PWL;
    high = SID_V1_PWH;
  } else if (voice == 1) {
    low = SID_V2_PWL;
    high = SID_V2_PWH;
  } else if (voice == 2) {
    low = SID_V3_PWL;
    high = SID_V3_PWH;
  }
  voicePulseWidth(low, high, frequency);
}

void MOS6581::filterFrequency(word frequency) {
  // shift the number along 5 bits to get the high frequency
  transfer(SID_FL_FL, lowByte(frequency) & B00000111);
  transfer(SID_FL_FH, highByte(frequency << 5));
}

void MOS6581::filterResonance(byte frequency) {
  filter_register &= B00001111;
  filter_register |= (frequency << 4);
  transfer(SID_FL_RES_CT, filter_register);
}

void MOS6581::_setFilterOnOff(byte voice, byte *reg, boolean on) {
  if (on) {
    *reg |= voice;  
  } else {
    *reg &= ~voice;
  }
}

void MOS6581::setFilter(byte voice, boolean on) {
  byte filter = 0;
  if (voice == 0) {
    filter = SID_FILT_VOICE1;
  } else if (voice == 1) {
    filter = SID_FILT_VOICE2;
  } else if (voice == 2) {
    filter = SID_FILT_VOICE3;
  }
  _setFilterOnOff(filter, &filter_register, on);
  transfer(SID_FL_RES_CT, filter_register);
}

void MOS6581::_setVoiceOnOff(byte *reg, boolean on) {
  if (on) {
    *reg |= 1;
  } else {
    *reg &= 0xfe; // ~1
  }
}

void MOS6581::setVoice(byte voice, boolean on) {
  byte *reg = 0;
  byte ct = 0;
  if (voice == 0) {
    reg = &voice1_register;
    ct = SID_V1_CT;
  } else if (voice == 1) {
    reg = &voice2_register;
    ct = SID_V2_CT;
  } else if (voice == 2) {
    reg = &voice3_register;
    ct = SID_V3_CT;
  }
  _setVoiceOnOff(reg, on);
  transfer(ct, *reg);
}

void MOS6581::setMode(byte voice, byte mode) {
  byte *reg = 0;
  byte ct = 0;
  if (voice == 0) {
    reg = &voice1_register;
    ct = SID_V1_CT;
  } else if (voice == 1) {
    reg = &voice2_register;
    ct = SID_V2_CT;
  } else if (voice == 2) {
    reg = &voice3_register;
    ct = SID_V3_CT;    
  }
  *reg &= B00000001;
  *reg |= mode;
  transfer(ct, *reg);
}

void MOS6581::setADEnvelope(byte voice, byte attack, byte decay) {
  byte ad = 0;
  if (voice == 0) {
    ad = SID_V1_AD;
  } else if (voice == 1) {
    ad = SID_V2_AD;
  } else if (voice == 2) {
    ad = SID_V3_AD;
  }
  transfer(ad, (decay & B00001111) | (attack << 4));
}

void MOS6581::setSREnvelope(byte voice, byte sustain, byte release) {
  byte sr = 0;
  if (voice == 0) {
    sr = SID_V1_SR;
  } else if (voice == 1) {
    sr = SID_V2_SR;
  } else if (voice == 2) {
    sr = SID_V3_SR;
  }
  transfer(sr, (release & B00001111) | (sustain << 4));
}

void MOS6581::volume(byte level) {
  mode_register &= B11110000;
  mode_register |= (level & B00001111);
  transfer(SID_FL_MD_VL, mode_register);
}

void MOS6581::setFilterMode(byte mode) {
  mode_register &= B00001111;
  mode_register |= mode;
  transfer(SID_FL_MD_VL, mode_register);
}
