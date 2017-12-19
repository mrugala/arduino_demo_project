#pragma once

#include <Arduino.h>

const static unsigned MAIN_LOOP_INTERVAL    = 20;
const static unsigned long SERIAL_BAUD_RATE = 115200;
const static unsigned EEPROM_ALLOCATED_SIZE = 1024;

#if defined (ARDUINO_ESP8266_WEMOS_D1MINI)
static unsigned LISTENING_PORT = 80;

//Safe to use: GPIO 4 (D2), 5(D1), 12(D6), 13(D7), 14(D5)
static const uint8_t CONTROL_OVERRIDE_PIN = D1;
static const uint8_t LIGHT_SWITCH_PIN_1   = D5;
static const uint8_t LIGHT_CONTROL_PIN_1  = D2;
static const uint8_t LIGHT_SWITCH_PIN_2   = D6;
static const uint8_t LIGHT_CONTROL_PIN_2  = D7;
#else
#define CONTROL_OVERRIDE_PIN   3
#define LIGHT_SWITCH_PIN_1     4
#define LIGHT_CONTROL_PIN_1    5
#define LIGHT_SWITCH_PIN_2     6
#define LIGHT_CONTROL_PIN_2    7
#endif
