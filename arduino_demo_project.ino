#include <LiquidCrystal.h>
#include <Servo.h>

#include "project_defines.h"
#include "hc_sr04.h"
#include "joy.h"
#include "mcp9700a.h"

HC_SR04 hc_sr04(TRIGGER_PIN, ECHO_PIN);
LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
JoyStick joy(JOY_HORZ_PIN, JOY_VERT_PIN, JOY_SEL_PIN);
Mcp9700A thermometer(TEMP_PIN);
Servo servo;

bool toggle_joy {false};
bool printToSerial {false};
bool hc_init_successful {true};

//double position = 90;

void setup() {
    pinMode(LED_PIN, OUTPUT);
//    servo.attach(SERVO_PIN);
//    pinMode(SPEAKER_PIN, OUTPUT); 
    Serial.begin(9600);
    if (hc_sr04.init() < 0)
    {
        Serial.println("HC-SR04 init failed");
        hc_init_successful = false;
    }
    
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("HC-SR04 distance:");
    lcd.blink();
    
    if (joy.init() < 0)
        Serial.println("joystick init failed");
//    if (thermometer.init() < 0)
//        Serial.println("MCP9700A init failed");
}

void loop() {
    delay(200);

    double dist = 0;
    if (hc_init_successful)
    {
        dist = hc_sr04.getDistance_cm();
        if (dist <= 30.)                               
        {
            blinkLed();
        }
        else
        {
            digitalWrite(LED_PIN, LOW);
        }
    }
    
//    if (toggle_joy)
//        analogWrite(SPEAKER_PIN, 100);
//    else
//        digitalWrite(SPEAKER_PIN, LOW);

      if (toggle_joy)
          digitalWrite(LED_PIN, HIGH);
      else
          digitalWrite(LED_PIN, LOW);

//    double horizontal = joy.getHorizontalDeviation();
//    double vertical = joy.getVerticalDeviation();
//    double temp = thermometer.getTemperature_C();
//    double angle = joy.getAngle_deg();

//    position = (angle)/2 + 90;
    
//    servo.write(position);

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
            default:
                break;
        }
    }

    if (hc_init_successful)
    {
        if (printToSerial)
            Serial.println(dist);
        lcd.setCursor(0, 1);
        auto pos = lcd.print(dist);
        lcd.print("    ");
        lcd.setCursor(pos, 1);
    }
    if (joy.testSelect())
    {
        Serial.print("joy_sel: ");
        Serial.println(toggle_joy);
        toggle_joy = !toggle_joy;
    }
}

void blinkLed()
{
    static auto state = LOW;
    state = state == LOW ? HIGH : LOW;
    
    digitalWrite(LED_PIN, state);
}

void printHelp()
{
    Serial.println("Available options:");
    Serial.println("\th - print help message");
    Serial.println("\td - print debug messages");
    Serial.println("\ts - stop printing");
}


