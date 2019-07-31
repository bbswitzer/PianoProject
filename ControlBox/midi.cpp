#include <MIDIUSB.h>
#include "midi.h"
#include "serial.h"

void decodeMidi(uint8_t header, uint8_t byte1, uint8_t byte2, uint8_t byte3);

void checkForMidiUSB()
{
	midiEventPacket_t rx; //midi data struct from midiUSB libray 
	do
	{
		rx = MidiUSB.read(); //get queued info from USB
		if(rx.header != 0)
		{
			decodeMidi(rx.header, rx.byte1, rx.byte2, rx.byte3);
		}
	} while(rx.header != 0);
}

void decodeMidi(uint8_t header, uint8_t byte1, uint8_t byte2, uint8_t byte3)
{
	const uint8_t NOTE_ON_HEADER        = 9;
	const uint8_t NOTE_OFF_HEADER       = 8;
	const uint8_t CONTROL_CHANGE_HEADER = 8;
	const uint8_t SUSTAIN_STATUS_BYTE   = 176;
	const uint8_t MIN_NOTE_PITCH        = 21;
	//note that ESP32 does bounds checking
	switch(header)
	{
	case NOTE_ON_HEADER:
		sendSerialToMain(SerialConstants::NOTE_HEADER, byte2 - MIN_NOTE_PITCH, byte3);
		break;
	case NOTE_OFF_HEADER:
		sendSerialToMain(SerialConstants::NOTE_HEADER, byte2 - MIN_NOTE_PITCH, 0);
		break;
	case SUSTAIN_STATUS_BYTE:
		if(byte1 == SUSTAIN_STATUS_BYTE)
			sendSerialToMain(SerialConstants::SUSTAIN_HEADER, byte3, byte3);
		break;
	}
}
