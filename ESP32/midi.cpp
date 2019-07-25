#include "midi.h"
#include "note.h"
#include "sustain.h"
#include "settings.h"
#include "main.h"

void sendBluetoothToSerial(int lengthM, uint8_t* message);

void decodeBluetooth(int lengthM, uint8_t* message)
{
	const uint8_t MIN_NOTE_STATUS_BYTE = 128;
	const uint8_t MAX_NOTE_STATUS_BYTE = 159;
	const uint8_t BEGIN_NOTE_OFF_BYTE  = 143;
	const uint8_t CONTROL_CHANGE_BYTE  = 176;
	const uint8_t SUSTAIN_DATA_BYTE    = 64;
	const uint8_t MIN_NOTE_PITCH       = 21;
	const uint8_t MAX_NOTE_PITCH       = 108;
	const uint8_t MIN_NOTE_VELOCITY    = Setting::minNoteVelocity;
	const uint8_t MAX_NOTE_VELOCITY    = 127;
	const uint8_t RESET_LENGTH         = 152;
	const uint8_t RESET_LENGTH2        = 154;

	if(DEBUG_MODE) sendBluetoothToSerial(lengthM, message);

	for(int index = 1; index < lengthM; index++)
	{
		if(message[index] >= MIN_NOTE_STATUS_BYTE && message[index] <= MAX_NOTE_STATUS_BYTE)
		{
			for(int noteIndex = index + 1; noteIndex < lengthM; noteIndex+=2)
			{
				int velocityIndex = noteIndex + 1;
				//go through message to find notes
				if(message[noteIndex] >= MIN_NOTE_PITCH && message[noteIndex] <= MAX_NOTE_PITCH &&
					message[velocityIndex] <= MAX_NOTE_VELOCITY)
				{
					int note = message[noteIndex] - MIN_NOTE_PITCH;
					uint8_t velocity = message[velocityIndex];
					if(DEBUG_MODE) Serial.print("Detected note: ");
					if(DEBUG_MODE) Serial.print(note);
					if(DEBUG_MODE) Serial.print("with velocity: ");
					if(DEBUG_MODE) Serial.println(velocity);
					//check if the status byte is off -- anything less than 144 means off -- or if the velocity is 0
					if(message[index] <= BEGIN_NOTE_OFF_BYTE || velocity == 0)
						notes[note].prepareToSchedule(0);
					else if(velocity >= MIN_NOTE_VELOCITY)
						notes[note].prepareToSchedule(velocity);
				} else
					break;
				//break if note not detected (new status byte is found)
			}
		} else if(message[index] == CONTROL_CHANGE_BYTE && message[index + 1] == SUSTAIN_DATA_BYTE)
		{
			uint8_t sustainVelocityIndex = index + 2;
			//sustain.prepareToSchedule(message[sustainVelocityIndex]);
		}
	}

	//sytnhesia sends specific lengths of data when it stops playing a midi
	if(lengthM == RESET_LENGTH || lengthM == RESET_LENGTH2)
		resetAll();
}

void sendBluetoothToSerial(int lengthM, uint8_t* message)
{
	Serial.println("-----------------------");
	for(int index = 1; index < lengthM; index++)
	{
		Serial.println(message[index], DEC);
	}
}