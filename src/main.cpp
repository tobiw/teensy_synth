#include <Arduino.h>

#include <Audio.h>
#include <MIDI.h>
#include <Wire.h>
#include <SD.h>
#include <SerialFlash.h>
#include <USBHost_t36.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "midi_handler.h"
#include "multivoices.h"

#define DMPDIR "/dmp"

AudioControlSGTL5000 sgtl5000_1;

USBHost usb_host;
MIDIDevice usb_midi(usb_host);

Adafruit_SSD1306 display(128, 32, &Wire, A8);

MultiVoices *voices;

struct midi_info_display_t midi_info_display;

void setup() {
  delay(3000);

  memset(&midi_info_display, 0, sizeof (midi_info_display));

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.write("Hello World");
  display.display();

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
    static char display_buf[16];

    usb_host.Task();
    usb_midi.read(0);

    static unsigned long m = millis();
    m = millis();

    if ((m - last_arp_update) > 50) // TODO: make interval configurable
    {
        voices->updateArpegiator();
        last_arp_update = m;
    }

    if (midi_info_display.updated && (m - midi_info_display.last_updated) > 100) {
        if (midi_info_display.note != 0) {
            sprintf(display_buf, "Note %u %u", midi_info_display.note, midi_info_display.value);
        } else {
            sprintf(display_buf, "CC %u %u", midi_info_display.control, midi_info_display.value);
        }
        display.clearDisplay();
        display.setCursor(0, 0);
        display.write(display_buf);
        display.display();
        midi_info_display.updated = false;
    }
}
