#pragma once

#include <Arduino.h>
#include "string_operations.h"
#include "PrintManager.h"

class LiquidCrystal;

class LcdManager : public PrintManager<LiquidCrystal>
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

    using PrintManager<LiquidCrystal>::PrintManager;
    LcdManager(const LcdPins pins);
    virtual ~LcdManager() = default;
};





