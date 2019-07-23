#include "serial.h"
#include "shiftRegister.h"

extern const uint8_t SUSTAIN_HEADER = 202;

void checkForSerial()
{
	const byte NOTE_HEADER = 201;
	const byte SETTING_HEADER = 203;
	while(Serial1.available() > 2)
	{
		int header = Serial1.read();
		if(header == NOTE_HEADER)
		{
			int note     = Serial1.read();
			int velocity = Serial1.read();
			activateNote(note, velocity);
			if(DEBUG_MODE)
			{
				Serial.println(note);
				Serial.println(velocity);
				Serial.println("----------------");
			}
		} else
			if(header == SETTING_HEADER)
			{
				uint8_t value1 = Serial1.read();
				uint8_t value2 = Serial1.read();
				//Pro Micro knows that only one setting is being sent
				pwmPercent = value2;
			}
	}
}

void sendSerialToMain(byte header, byte setting, byte value)
{
	Serial1.print(header);
	Serial1.print(setting);
	Serial1.print(value);
}

void sendSerialToUSB(String* message, int lengthOfMessage)
{
	for(int index = 0; index < lengthOfMessage; index++)
		Serial.println(message[index]);
}

