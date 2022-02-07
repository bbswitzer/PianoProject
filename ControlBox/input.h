#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>

#define UP_PIN     4
#define DOWN_PIN   6
#define LEFT_PIN   7
#define RIGHT_PIN  5
#define RESET_PIN  8
#define VOLUME_PIN 10

extern unsigned long lastPressedOverall;

extern int lastAnalog;

void initializeInputs();
void checkForInput();

#endif