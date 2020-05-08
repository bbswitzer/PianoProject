#include <MIDIUSB.h>
#include "shiftRegister.h"
#include "serial.h"
#include "midi.h"

extern const bool DEBUG_MODE = false;

void setup()
{
	const int SHIFT_REGISTER_POWER_PIN = 19;
	
	intitializeRegisters();
	delay(2000);

	Serial.begin(38400);
	Serial1.begin(38400);

	pinMode(SHIFT_REGISTER_POWER_PIN, OUTPUT);
	digitalWrite(SHIFT_REGISTER_POWER_PIN, HIGH);
	pinMode(LED_BUILTIN, OUTPUT);

	if(DEBUG_MODE)
		testRegisters();
}

void loop()
{
	checkForMidiUSB();
	checkForSerial();
}
