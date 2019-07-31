#pragma GCC diagnostic push
#pragma GCC diagnostic warning "-fpermissive"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "sustain.h"
#include "settings.h"
#include "serial.h"
#include "note.h"
#include "midi.h"
#include "bluetooth.h"
#include "main.h"
#pragma GCC diagnostic pop
const bool DEBUG_MODE = false;

void resetAll()
{
	acceptMidi = false; //turn midi off during reset to prevent errorsg
	for(int noteIndex = 0; noteIndex < 88; noteIndex++)
	{
		notes[noteIndex].resetSchedule();
	}
	Note::resetInstances();
	sustain.resetSchedule();
	msReset();
	acceptMidi = true;
}

void flashLED()
{
	digitalWrite(LED_BUILTIN, HIGH);
	delay(100);
	digitalWrite(LED_BUILTIN, LOW);
	delay(100);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(100);
	digitalWrite(LED_BUILTIN, LOW);
	delay(100);
}

void setup()
{
	const int SHIFT_REGISTER_POWER_PIN = 12;
	const int SUSTAIN_PIN = 13;

	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(38400);
	initializeBluetooth();

	//create sustain PWM output. this can't be done by the Pro Micro because the shift registers are filled up
	ledcSetup(0, 100, 8);
	ledcAttachPin(SUSTAIN_PIN, 0);

	delay(500); //give pro micro time to inititalize before giving power to shift registers
	pinMode(SHIFT_REGISTER_POWER_PIN, OUTPUT);
	digitalWrite(SHIFT_REGISTER_POWER_PIN, HIGH);

	std::vector<unsigned long> testy[6];
}

void loop()
{
	checkForSerial();

	for(int noteIndex = 0; noteIndex < 88; noteIndex++)
	{
		notes[noteIndex].checkSchedule();
		notes[noteIndex].checkForErrors();
	}
	sustain.checkSchedule();
	sustain.checkForErrors();

	if(millis() >= nextReset)
	{
		//first reset will happen immediately and midi will begin to accept after this
		resetAll();
		nextReset = millis() + Setting::autoResetMs;
	}
}
