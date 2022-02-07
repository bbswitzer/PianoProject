#include "lcd.h"

void sendSerialToMain(byte header, byte setting, byte value)
{
	Serial1.write(header);
	Serial1.write(setting);
	Serial1.write(value);
}
