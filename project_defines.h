#pragma once

#include <Arduino.h>

//#define TRIGGER_PIN  12 //HC-SR04
//#define ECHO_PIN     11 //HC-SR04
//
//#define SPEAKER_PIN  3
//
//#define SERVO_PIN    9

//#define LCD_RS       8
//#define LCD_ENABLE   10
//#define LCD_D4       4
//#define LCD_D5       5
//#define LCD_D6       6
//#define LCD_D7       7

//IRQ
//#define JOY_SEL_PIN  2
#define IRQ3_10PF_ADDED

////Analog
//#define JOY_HORZ_PIN 0
//#define JOY_VERT_PIN 1
//#define TEMP_PIN     2

#if defined (ARDUINO_ESP8266_WEMOS_D1MINI)

static const uint8_t LIGHT_SWITCH_PIN     = D1;
static const uint8_t CONTROL_OVERRIDE_PIN = D2;
static const uint8_t LIGHT_CONTROL_PIN    = D3;
#else
#define LIGHT_SWITCH_PIN     3
#define CONTROL_OVERRIDE_PIN 4
#define LIGHT_CONTROL_PIN    5
#endif
