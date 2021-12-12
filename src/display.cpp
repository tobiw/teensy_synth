#include "display.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

extern struct midi_info_display_t midi_info_display;

Display::Display()
{
    ssd1306 = new Adafruit_SSD1306(128, 32, &Wire, A8);
}

void Display::begin()
{
    if(!ssd1306->begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }

    ssd1306->clearDisplay();
    ssd1306->setTextSize(1);
    ssd1306->setTextColor(WHITE);
    ssd1306->setCursor(0, 0);
    ssd1306->write("Hello World");
    ssd1306->display();

}

void Display::update(unsigned long m)
{
    static char display_buf[16];

    // If there is no update pending or interval from last update is too short, skip updating the display
    if (!midi_info_display.updated || (m - midi_info_display.last_updated) < 100) {
        return;
    }

    // Prepare buffer with Note or CC information
    if (midi_info_display.note != 0) {
        sprintf(display_buf, "Note %u %u", midi_info_display.note, midi_info_display.value);
    } else {
        sprintf(display_buf, "CC %u %u", midi_info_display.control, midi_info_display.value);
    }

    // Clear and redraw display
    ssd1306->clearDisplay();
    ssd1306->setCursor(0, 0);
    ssd1306->write(display_buf);
    ssd1306->display();

    midi_info_display.updated = false;
}
