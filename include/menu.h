#ifndef __MENU_H_
#define __MENU_H_

class Adafruit_SSD1306;
class MenuPage;

class Menu
{
    MenuPage *current_page;
    int current_page_index;
    unsigned long last_update;

public:
    Menu();
    void draw(Adafruit_SSD1306 *display);
    void rotate_left();
    void rotate_right();
    void select();
};

class MenuPage
{
protected:
    int current_selection;
    int max_num_selections;

public:
    MenuPage();
    inline int get_selection() { return current_selection; }
    void reset();
    void select();
    virtual void increase_value() = 0;
    virtual void decrease_value() = 0;
    virtual void draw(Adafruit_SSD1306 *display) = 0;
};

class MenuPageFilter : public MenuPage
{
    int filter_freq, resonance;

public:
    MenuPageFilter();
    void increase_value();
    void decrease_value();
    void draw(Adafruit_SSD1306 *display);
};

class MenuPageOsc : public MenuPage
{
    int waveform, freq;

public:
    MenuPageOsc();
    void increase_value();
    void decrease_value();
    void draw(Adafruit_SSD1306 *display);
};

class MenuPageSettings : public MenuPage
{
public:
    MenuPageSettings();
    void increase_value();
    void decrease_value();
    void draw(Adafruit_SSD1306 *display);
};

#endif
