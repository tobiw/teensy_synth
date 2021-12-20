#include "menu.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define NUM_PAGES 3
static MenuPage *pages[3];

Menu::Menu()
{
    last_update = 0;

    pages[0] = new MenuPageFilter;
    pages[1] = new MenuPageOsc;
    pages[2] = new MenuPageSettings;

    current_page_index = 0;
    current_page = pages[current_page_index];
}

void Menu::draw(Adafruit_SSD1306 *display)
{
    const unsigned long m = millis();
    if (m - last_update > 250) {
        current_page->draw(display);
        last_update = m;
    }
}

void Menu::rotate_left()
{
    if (current_page->get_selection() == 0) { // page title is selected
        if (current_page_index == 0) return;
        current_page = pages[--current_page_index];
        current_page->reset();
    } else { // a parameter on the page is selected
        current_page->decrease_value();
    }
}

void Menu::rotate_right()
{
    if (current_page->get_selection() == 0) { // page title is selected
        if (current_page_index == NUM_PAGES - 1) return;
        current_page = pages[++current_page_index];
        current_page->reset();
    } else { // a parameter on the page is selected
        current_page->increase_value();
    }
}

void Menu::select()
{
    current_page->select();
}


MenuPage::MenuPage()
{
    current_selection = 0;
    max_num_selections = 0;
}

void MenuPage::reset()
{
    current_selection = 0;
}

void MenuPage::select()
{
    current_selection++;
    if (current_selection >= max_num_selections) current_selection = 0;
}


MenuPageFilter::MenuPageFilter()
{
    max_num_selections = 3;
    filter_freq = 1000;
    resonance = 6;
}

void MenuPageFilter::draw(Adafruit_SSD1306 *display)
{
    // TODO: simplify writing menus, use common code (add menu items, ...)
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(WHITE);
    display->setCursor(0, 0);
    display->write(current_selection == 0 ? ">" : " ");
    display->write("Filter");
    display->setCursor(0, 8);
    display->write(current_selection == 1 ? ">" : " ");
    display->print(filter_freq);
    display->write(" ");
    display->write(current_selection == 2 ? ">" : " ");
    display->print(resonance);
    display->display();
}

void MenuPageFilter::decrease_value()
{
    // TODO: pass changed values back to app?
    switch (current_selection) {
        case 1:
            filter_freq--;
            break;
        case 2:
            resonance--;
            break;
    }
}

void MenuPageFilter::increase_value()
{
    switch (current_selection) {
        case 1:
            filter_freq++;
            break;
        case 2:
            resonance++;
            break;
    }
}


MenuPageOsc::MenuPageOsc()
{
    max_num_selections = 3;
    waveform = 0; // 0 = sine, 1 = triangle, 2 = square
    freq = 440;
}

void MenuPageOsc::draw(Adafruit_SSD1306 *display)
{
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(WHITE);
    display->setCursor(0, 0);
    display->write(current_selection == 0 ? ">" : " ");
    display->write("Osc1");
    display->setCursor(0, 8);
    display->write(current_selection == 1 ? ">" : " ");
    if (waveform == 0) display->write("SIN");
    else if (waveform == 1) display->write("TRI");
    else if (waveform == 2) display->write("SQU");
    display->write(" ");
    display->write(current_selection == 2 ? ">" : " ");
    display->print(freq);
    display->display();
}

void MenuPageOsc::decrease_value()
{
    switch (current_selection) {
        case 1:
            waveform = waveform > 0 ? waveform - 1 : 0;
            break;
        case 2:
            freq--;
            break;
    }
}

void MenuPageOsc::increase_value()
{
    switch (current_selection) {
        case 1:
            waveform = waveform < 2 ? waveform + 1 : 2;
            break;
        case 2:
            freq++;
            break;
    }
}


MenuPageSettings::MenuPageSettings()
{
    max_num_selections = 0;
}

void MenuPageSettings::draw(Adafruit_SSD1306 *display)
{
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(WHITE);
    display->setCursor(0, 0);
    display->write("Settings");
    display->setCursor(0, 8);
    display->write("....");
    display->setCursor(0, 16);
    display->write("... ...");
    display->display();
}

void MenuPageSettings::decrease_value()
{
}

void MenuPageSettings::increase_value()
{
}
