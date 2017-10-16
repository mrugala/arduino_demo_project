#pragma once

#include <Arduino.h>
#include "string_operations.h"

class LiquidCrystal;

class LcdManager
{
public:
    struct LcdPins
    {
        const unsigned rs;
        const unsigned enable;
        const unsigned d4;
        const unsigned d5;
        const unsigned d6;
        const unsigned d7;
    };

    LcdManager(const LcdPins pins);
    LcdManager(LiquidCrystal* lcd);
    virtual ~LcdManager();

    int init(const unsigned cols, const unsigned rows, const unsigned screens);
    
    template<typename... Args>
    void print(const unsigned screen, const unsigned row, Args... args)
    {
         display_content[screen][row] = StringOps::getString(args...);
    }
    void goToScreen(const unsigned screen);

private:
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

    LiquidCrystal* lcd;

    String getContent(const unsigned screen, const unsigned row, const unsigned col);
};





