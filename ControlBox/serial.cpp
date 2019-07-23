#include "lcd.h"

namespace SerialConstants
{
	extern const byte NOTE_HEADER    = 201;
	extern const byte SUSTAIN_HEADER = 202;
	extern const byte SETTING_HEADER = 203;
	extern const byte RESET_HEADER   = 204;
	extern const byte VOLUME_HEADER  = 205;
}

void sendSerialToMain(byte header, byte setting, byte value)
{
	Serial1.print(header);
	Serial1.print(setting);
	Serial1.print(value);
}