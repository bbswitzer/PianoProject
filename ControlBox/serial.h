#ifndef SERIAL_H
#define SERIAL_H

#include <Arduino.h>

namespace SerialConstants
{
	extern const uint8_t NOTE_HEADER;
	extern const uint8_t SUSTAIN_HEADER;
	extern const uint8_t SETTING_HEADER;
	extern const uint8_t RESET_HEADER;
	extern const uint8_t VOLUME_HEADER;
}

void sendSerialToMain(uint8_t header, uint8_t setting, uint8_t value);
void sendAllSettings();

#endif