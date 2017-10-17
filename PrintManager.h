#pragma once

#include <Arduino.h>
#include "string_operations.h"

/*
 *   Public interface of the PrintDevice template parameter has to include the following:
 *    * begin(cols, rows)   - doesn't need to do anything, needs to be present just for compatibility with LiquidCrystal class
 *    * setCursor(col, row) - to set the cursor position, if there's possibility to display a cursor on the screen, otherwise can do nothing
 *    * print(data)         - to print the given String object to the device
 */

template<class PrintDevice>
class PrintManager
{
protected:
    static const unsigned MAX_LINE_LENGTH     = 0xFF;
    static const unsigned MAX_NUMBER_OF_LINES = 0xF;
  
public:
    PrintManager(PrintDevice* dev) : dev(dev)
    {
        this->current_position.screen = 0;
        this->current_position.row = 0;
        this->current_position.col = 0;
    }
    
    virtual ~PrintManager()
    {
        if (this->was_initialized)
        {
            for (unsigned screen = 0; screen < screens; ++screen)
                delete[] this->display_content[screen];
            delete[] this->display_content;
        }
    
        delete this->dev;
    }

    int init(const unsigned cols, const unsigned rows, const unsigned screens)
    {
        this->cols = cols;
        this->rows = rows;
        this->screens = screens;
    
        display_content = new String*[screens];
        for (unsigned screen = 0; screen < screens; ++screen)
            this->display_content[screen] = new String[MAX_NUMBER_OF_LINES];
    
        this->was_initialized = true;
        this->dev->begin(cols, rows);
        this->goToScreen(0);
    
        return 0;
    }
    
    template<typename... Args>
    void print(const unsigned screen, const unsigned row, Args... args)
    {
        this->display_content[screen][row] = StringOps::getString(args...);
        if (this->current_position.screen == screen)
        {
            this->printImpl(screen);
        }
    }
    
    void goToScreen(const unsigned screen)
    {
        if (this->current_position.screen != screen)
        {
            this->current_position.screen = screen;
            this->printImpl(screen);
        }
    }

protected:
    String** display_content;
    unsigned cols;
    unsigned rows;
    unsigned screens;

    struct { 
        unsigned col;
        unsigned row;
        unsigned screen;
    } current_position;

    bool was_initialized {false};

    PrintDevice* dev;

    void printImpl(const unsigned screen)
    {
        unsigned display_line = 0;
        for (unsigned row = this->current_position.row; row < this->current_position.row + this->rows; row++)
        {
            this->dev->setCursor(0, display_line);
            this->dev->print(getContent(screen, row, 0));
        }
    }
    
    String getContent(const unsigned screen, const unsigned row, const unsigned col)
    {
        auto line_length = this->display_content[screen][row].length();
        String ret;
        if (line_length >= this->cols)
            ret = this->display_content[screen][row].substring(col, col+MAX_LINE_LENGTH);
        else
        {
            ret = this->display_content[screen][row];
            for (auto col = line_length; col < MAX_LINE_LENGTH; ++col)
                ret = ret + " ";
        }
        return ret;
    }
};





