#include "LcdManager.h"
#include "Arduino.h"
#include "project_defines.h"

#define MAX_LINE_LENGTH 0xFF

LcdManager::LcdManager(const LcdPins pins)
    : lcd(pins.rs, pins.enable, pins.d4, pins.d5, pins.d6, pins.d7)
{

}

LcdManager::~LcdManager()
{
    if (was_initialized)
    {
        for (unsigned screen = 0; screen < screens; ++screen)
            delete[] display_content[screen];
        delete[] display_content;
    }
}

int LcdManager::init(const unsigned cols, const unsigned rows, const unsigned screens)
{
    this->cols = cols;
    this->rows = rows;
    this->screens = screens;

    display_content = new String*[screens];
    for (unsigned screen = 0; screen < screens; ++screen)
        display_content[screen] = new String[rows];

    was_initialized = true;
    lcd.begin(cols, rows);

    return 0;
}

