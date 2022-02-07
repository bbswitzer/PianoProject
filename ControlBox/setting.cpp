#include <EEPROM.h>
#include "setting.h"
#include "lcd.h"
#include "serial.h"
#include "input.h"

void changeSetting(int changeBy)
{
	if(currentMenu != static_cast<int>(SettingID::SCHEDULE_NOTES) &&
	   currentMenu != static_cast<int>(SettingID::HANDLE_NOTES)) //if current menu is not a bool menu
		EEPROM.write(currentMenu, EEPROM.read(currentMenu) + changeBy);
	else
		EEPROM.write(currentMenu, !(static_cast<bool>(EEPROM.read(currentMenu))));
}

void sendAllSettings()
{
	for(int index = 0; index < NUM_OF_MENUS; index++)
	{
		sendSerialToMain(SETTING_HEADER, index, EEPROM.read(index));
		delay(50);
	}
	sendSerialToMain(VOLUME_HEADER, lastAnalog, lastAnalog);
}
