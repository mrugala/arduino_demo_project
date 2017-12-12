#pragma once

#include <Arduino.h>

const static unsigned MAIN_LOOP_INTERVAL    = 20;
const static unsigned long SERIAL_BAUD_RATE = 115200;
const static unsigned EEPROM_ALLOCATED_SIZE = 1024;

#if defined (ARDUINO_ESP8266_WEMOS_D1MINI)
static unsigned LISTENING_PORT = 80;

//Safe to use: GPIO 4 (D2), 5(D1), 12(D6), 13(D7), 14(D5)
static const uint8_t LIGHT_SWITCH_PIN     = D5;
static const uint8_t CONTROL_OVERRIDE_PIN = D1;
static const uint8_t LIGHT_CONTROL_PIN    = D2;
#else
#define LIGHT_SWITCH_PIN     3
#define CONTROL_OVERRIDE_PIN 4
#define LIGHT_CONTROL_PIN    5
#endif
