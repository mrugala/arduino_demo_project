#include <LiquidCrystal.h>
#include <Servo.h>

#include "project_defines.h"
#include "ControlModule.h"

//HC_SR04 hc_sr04(TRIGGER_PIN, ECHO_PIN);
//LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
//JoyStick joy(JOY_HORZ_PIN, JOY_VERT_PIN, JOY_SEL_PIN);
//Mcp9700A thermometer(TEMP_PIN);

ControlModule cm(CONTROL_OVERRIDE_PIN, LIGHT_CONTROL_PIN, LIGHT_SWITCH_PIN);

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
//    if (hc_sr04.init() < 0)
//    {
//        Serial.println("HC-SR04 init failed");
//        hc_init_successful = false;
//    }
    
//    lcd.begin(16, 2);
//    lcd.setCursor(0, 0);
//    lcd.print("HC-SR04 distance:");
//    lcd.blink();
    
//    if (joy.init() < 0)
//        Serial.println("joystick init failed");
//    if (thermometer.init() < 0)
//        Serial.println("MCP9700A init failed");

    Serial.print("Light switch control module init... ");
    if (cm.init() < 0)
    {
        Serial.println("FAILURE");
    }
    else
    {
        Serial.println("SUCCESS");
    }
}

bool printToSerial {false};

void loop() 
{
    delay(MAIN_LOOP_INTERVAL);

    if (Serial.available() > 0) 
    {
        auto read_byte = Serial.read();
        switch (read_byte)
        {
            case 'd':
                printToSerial = true;
                break;
            case 's':
                printToSerial = false;
                break;
            case 'h':
                printHelp();
                break;
            case '-':
                cm.setState(false);
                if (printToSerial)
                    Serial.println("CM/Info: Light turned OFF");
                break;
            case '+':
                cm.setState(true);
                if (printToSerial)
                    Serial.println("CM/Info: Light turned ON");
                break;    
            default:
                if (printToSerial)
                    Serial.println("Error: Invalid input");
                break;
        }
    }

    auto prev_state = cm.getDeviceState();
    auto current_state = cm.updateState();
    if (prev_state != current_state && printToSerial)
    {
        Serial.print("CM/Info: Light turned ");
        Serial.print(current_state ? "ON" : "OFF");
        Serial.println(" using a light switch");
    }
}

void blinkLed()
{
    static auto state = LOW;
    state = state == LOW ? HIGH : LOW;
    
    digitalWrite(LED_BUILTIN, state);
}

void printHelp()
{
    Serial.println("Available options:");
    Serial.println("\th - print this help message");
    Serial.println("\td - print debug messages");
    Serial.println("\ts - stop printing");
    Serial.println("\t- - turn light off");
    Serial.println("\t+ - turn light on");
}


