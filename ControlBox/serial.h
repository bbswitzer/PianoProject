#ifndef SERIAL_H
#define SERIAL_H

#include <Arduino.h>

#define NOTE_HEADER    201
#define SUSTAIN_HEADER 202
#define SETTING_HEADER 203
#define RESET_HEADER   204
#define VOLUME_HEADER  205

void sendSerialToMain(uint8_t header, uint8_t setting, uint8_t value);
void sendAllSettings();

#endif