#ifndef __DISPLAY_H_
#define __DISPLAY_H_

#include "midi_handler.h"

class Adafruit_SSD1306;
class Menu;

class Display
{
    Adafruit_SSD1306 *ssd1306;
    struct midi_info_display_t midi_info_display;

public:
    Display();
    Adafruit_SSD1306 *get_ssd1306() { return ssd1306; }
    void begin();
    void update(unsigned long m);
};

#endif
