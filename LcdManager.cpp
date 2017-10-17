#include "LcdManager.h"
#include <LiquidCrystal.h>
#include "project_defines.h"

LcdManager::LcdManager(const LcdPins pins)
    : LcdManager(new LiquidCrystal(pins.rs, pins.enable, pins.d4, pins.d5, pins.d6, pins.d7))
{
}

