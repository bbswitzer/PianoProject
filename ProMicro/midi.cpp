#include <MIDIUSB.h>
#include "midi.h"
#include "shiftRegister.h"
#include "serial.h"

void decodeMidi(uint8_t header, uint8_t byte1, uint8_t byte2, uint8_t byte3);

extern const bool DEBUG_MODE;

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
	if(DEBUG_MODE)
	{
		String message[] =
		{
		  "-----------",
		  static_cast<String>(header),
		  static_cast<String>(byte1),
		  static_cast<String>(byte2),
		  static_cast<String>(byte3),
		  "-----------"
		};
		sendSerialToUSB(message, 6);
	}

	const uint8_t NOTE_ON_HEADER        = 9;
	const uint8_t NOTE_OFF_HEADER       = 8;
	const uint8_t CONTROL_CHANGE_HEADER = 8;
	const uint8_t SUSTAIN_STATUS_BYTE   = 176;
	const uint8_t MIN_NOTE_PITCH        = 21;
	const uint8_t MAX_NOTE_PITCH        = 108;
	switch(header)
	{
	case NOTE_ON_HEADER:
		if(byte2 >= MIN_NOTE_PITCH && byte2 <= MAX_NOTE_PITCH &&
		   byte3 >= 0 && byte3 <= 127)
		{
			uint8_t note = (byte2 - MIN_NOTE_PITCH) * -1 + 87;
			activateNote(note, byte3);
		}
		break;
	case NOTE_OFF_HEADER:
		if(byte2 >= MIN_NOTE_PITCH && byte2 <= MAX_NOTE_PITCH)
		{
			uint8_t note = (byte2 - MIN_NOTE_PITCH) * -1 + 87;
			activateNote(note, 0);
		}
		break;
	case SUSTAIN_STATUS_BYTE:
		if(byte1 == SUSTAIN_STATUS_BYTE)
		{
			extern const uint8_t SUSTAIN_HEADER;
			sendSerialToMain(SUSTAIN_HEADER, byte3, byte3);
		}
		break;
	}
}