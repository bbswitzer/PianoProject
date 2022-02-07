#include <MIDIUSB.h>
#include "midi.h"
#include "serial.h"

void decodeMidi(uint8_t header, uint8_t channel, uint8_t data1, uint8_t data2);

void checkForMidiUSB() {
	midiEventPacket_t rx; //midi data struct from midiUSB library 
	do {
		rx = MidiUSB.read(); //get queued info from USB
		if (rx.header != 0) {
			decodeMidi(rx.header, rx.byte1, rx.byte2, rx.byte3);
		}
	} while (rx.header != 0);
}

void decodeMidi(uint8_t header, uint8_t channel, uint8_t data1, uint8_t data2) {
	//note that ESP32 does bounds checking
	switch (header) {
		case NOTE_ON_HEADER:
			sendSerialToMain(NOTE_HEADER, data1 - MIN_NOTE_PITCH, data2);
			break;

		case NOTE_OFF_HEADER:
			sendSerialToMain(NOTE_HEADER, data1 - MIN_NOTE_PITCH, 0);
			break;

		case CONTROL_CHANGE_HEADER:
			if (data1 == SUSTAIN_STATUS_BYTE)
				sendSerialToMain(SUSTAIN_HEADER, data2, 127);
			if (data1 == 120 || data1 == 123) // reset notes(panic) signal
				sendSerialToMain(RESET_HEADER, 127, 127);
			break;
	}
}
