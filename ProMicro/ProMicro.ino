//Latchpin: Pin 10 goes to second register input
//Datapin:  Pin 16 goes to first register input
//Clockpin: Pin 15 goes to third register input
const int ShiftPWM_latchPin=18; //values assigned before includes
const bool ShiftPWM_invertOutputs = false;
const bool ShiftPWM_balanceLoad = false;
#include "shiftRegister.h"
#include "serial.h"
#include "midi.h"
#include <pins_arduino_compile_time.h>
#include <CShiftPWM.h>
#include <ShiftPWM.h>
#include <frequencyToNote.h>
#include <MIDIUSB.h>
#include <pitchToFrequency.h>
#include <pitchToNote.h>
#include "midi.h"
#include "serial.h"
#include "shiftRegister.h"

const bool DEBUG_MODE = false;

void setup()
{
	intitializeRegisters();
	delay(2000);

	Serial.begin(38400);
	Serial1.begin(38400);

	pinMode(19, OUTPUT);
	pinMode(20, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(19, HIGH);
	digitalWrite(20, HIGH);

	if(DEBUG_MODE)
		testRegisters();
}

void loop()
{
	checkForMidiUSB();
	checkForSerial();
}