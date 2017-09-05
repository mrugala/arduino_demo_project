#include "LcdManager.h"
#include <LiquidCrystal.h>
#include "project_defines.h"

#define MAX_LINE_LENGTH     0xFF
#define MAX_NUMBER_OF_LINES 0xF

LcdManager::LcdManager(const LcdPins pins)
    : lcd(new LiquidCrystal(pins.rs, pins.enable, pins.d4, pins.d5, pins.d6, pins.d7))
{
    current_position.screen = 0;
    current_position.row = 0;
    current_position.col = 0;
}

LcdManager::~LcdManager()
{
    if (was_initialized)
    {
        for (unsigned screen = 0; screen < screens; ++screen)
            delete[] display_content[screen];
        delete[] display_content;
    }

    delete lcd;
}

int LcdManager::init(const unsigned cols, const unsigned rows, const unsigned screens)
{
    this->cols = cols;
    this->rows = rows;
    this->screens = screens;

    display_content = new String*[screens];
    for (unsigned screen = 0; screen < screens; ++screen)
        display_content[screen] = new String[MAX_NUMBER_OF_LINES];

    was_initialized = true;
    lcd->begin(cols, rows);
    goToScreen(0);

    return 0;
}

void LcdManager::goToScreen(const unsigned screen)
{
    if (current_position.screen != screen)
    {
        current_position.screen = screen;
        unsigned display_line = 0;
        for (unsigned row = current_position.row; row < current_position.row + this->rows; row++)
        {
            lcd->setCursor(0, display_line);
            lcd->print(getContent(screen, row, 0));
        }
    }
}

String LcdManager::getContent(const unsigned screen, const unsigned row, const unsigned col)
{
    auto line_length = display_content[screen][row].length();
    String ret;
    if (line_length >= this->cols)
        ret = display_content[screen][row].substring(col, col+MAX_LINE_LENGTH);
    else
    {
        ret = display_content[screen][row];
        for (auto col = line_length; col < MAX_LINE_LENGTH; ++col)
            ret = ret + " ";
    }
    return ret;
}

