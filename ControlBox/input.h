#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>

extern unsigned long lastPressedOverall;

extern int lastAnalog;

void initializeInputs();
void checkForInput();

#endif