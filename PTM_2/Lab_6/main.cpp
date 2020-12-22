#include <Servo.h>

#define NUM_LED 8
#define START_LED_PIN 14
#define START_SWITCH_PIN 6
#define CANCEL_MARK 'c'
#define PROGRAM_CYCLE 100

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

Servo my_servo;
String newLeds = "00000000", oldLeds = "00000000", odebraneDane, menu;
uint8_t servo_kat = 0;
int analogValue = 0, licznikAD = 0;

void printMenu() {
    Serial.println("##################");
    Serial.println("Menu Programu:");
    Serial.println("1. Diody led");
    Serial.println("2. Pomiar napiecia");
    Serial.println("3. Obsługa serwo");
    Serial.println("4. Wyswietl menu");
    Serial.println("##################");
}

void setup() {
    Serial.begin(9600);
    for (uint8_t i = START_LED_PIN; i < START_LED_PIN + NUM_LED; i++)
        pinMode(i, OUTPUT);

    for (uint8_t i = START_SWITCH_PIN; i < START_SWITCH_PIN + NUM_LED; i++)
        pinMode(i, INPUT);
    Serial.println("Laboratorium 2.6 - 248926");
    my_servo.attach(2);
}

void loop() {

    switch (menu[0]) {
    case '1':
        for (uint8_t i = START_SWITCH_PIN; i < START_SWITCH_PIN + NUM_LED; i++)
            newLeds[i - START_SWITCH_PIN] = (digitalRead(i) == HIGH) ? '1' : '0';

        for (uint8_t i = START_LED_PIN; i < START_LED_PIN + NUM_LED; i++)
            if (newLeds[i - START_LED_PIN] == '1')
                digitalWrite(i, HIGH);
            else
                digitalWrite(i, LOW);

        if (newLeds != oldLeds) {
            Serial.println("Ustawiono: " + newLeds);
            oldLeds = newLeds;
        }

        if (Serial.available() > 0) {
            odebraneDane = Serial.readStringUntil('\n');
            if (odebraneDane[0] == CANCEL_MARK) {
                for (uint8_t i = START_LED_PIN; i < START_LED_PIN + NUM_LED; i++)
                    digitalWrite(i, LOW);
                menu[0] = CANCEL_MARK;
            }
        }
        break;
    case '2':
        if (licznikAD > PROGRAM_CYCLE * 10) {
            analogValue = analogRead(0);
            Serial.println("Napiecie: " + String(mapfloat(analogValue, 0, 1023, 0.0, 5.0)) + "V");
            licznikAD = 0;
        }
        licznikAD += PROGRAM_CYCLE;
        if (Serial.available() > 0) {
            odebraneDane = Serial.readStringUntil('\n');
            if (odebraneDane[0] == CANCEL_MARK) {
                licznikAD = 0;
                menu[0] = CANCEL_MARK;
            }
        }
        break;
    case '3':
        if (Serial.available() > 0) {
            odebraneDane = Serial.readStringUntil('\n');
            if (odebraneDane[0] == CANCEL_MARK) {
                servo_kat = 0;
                menu[0] = CANCEL_MARK;
            } else {
                servo_kat = map(odebraneDane.toInt(), 0, 100, 0, 180);
                Serial.println("Kat: " + String(servo_kat));
            }
        }
        my_servo.write(servo_kat);
        break;
    default:
        printMenu();
        while (Serial.available() < 1)
            delay(10);
        menu = Serial.readStringUntil('\n');
        switch (menu[0]) {
        case '1':
            Serial.println("Wybrano program diody");
            break;
        case '2':
            Serial.println("Wybrano program pomiar napiecia");
            break;
        case '3':
            Serial.println("Wybrano program Serwo");
            break;
        }
    }

    delay(PROGRAM_CYCLE);
}