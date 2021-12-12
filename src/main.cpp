#include <Arduino.h>

#include <Audio.h>
#include <MIDI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <USBHost_t36.h>

#include "midi_handler.h"
#include "multivoices.h"
#include "display.h"

AudioControlSGTL5000 sgtl5000_1;

USBHost usb_host;
MIDIDevice usb_midi(usb_host);

MultiVoices *voices;
Display display;

struct midi_info_display_t midi_info_display;

void setup() {
  delay(3000);

  memset(&midi_info_display, 0, sizeof (midi_info_display));

  display.begin();

  AudioMemory(10);
  
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.4);
  sgtl5000_1.enhanceBassEnable();
  sgtl5000_1.enhanceBass(0.5, 2.5);

  usb_host.begin();
  usb_midi.setHandleControlChange(handleMidiControlChange);
  usb_midi.setHandleNoteOn(handleMidiNoteOn);
  usb_midi.setHandleNoteOff(handleMidiNoteOff);
  
  int ml = millis();
  while (!Serial && (millis()-ml < 1000)) ; // wait for Arduino Serial Monitor

  voices = new MultiVoices();

  Serial.println("initialization done.");
}


void loop() {
    static unsigned long last_arp_update = 0;

    usb_host.Task();
    usb_midi.read(0);

    static unsigned long m = millis();
    m = millis();

    if ((m - last_arp_update) > 50) // TODO: make interval configurable
    {
        voices->updateArpegiator();
        last_arp_update = m;
    }

    display.update(m);
}
