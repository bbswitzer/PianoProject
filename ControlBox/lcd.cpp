#include <EEPROM.h>
#include "LiquidCrystal_I2C.h"
#include "lcd.h"
#include "input.h"
#include "setting.h"

extern LiquidCrystal_I2C lcd;

const String MENU_NAMES[] ={
  "Handle Notes",
  "Schedule Notes",
  "Min Accepted Vel",
  "Solenoid PWM %",
  "Min Startup Ms",
  "Max Startup Ms",
  "Velocity Var",
  "Min Off Ms",
  "Max Off Ms",
  "Fast Off Ms",
  "Pedal On Ms",
  "Pedal Off Ms",
  "Note Timeout",
  "Sustain Timeout",
  "Auto Reset Mins",
  "Max Left Keys",
  "Max Right Keys",
	"Force wifi AP"
};

MenuStates       menuState = MenuStates::WELCOME;
int              currentMenu = -1; //initialize current menu as invalid
const int        NUM_OF_MENUS         = sizeof(MENU_NAMES) / sizeof(MENU_NAMES[0]);
const int        SETTING_MENU_TIMEOUT = 20000;
extern const int SPECIAL_MENU_TIMEOUT = 3000;
unsigned long    exitScreen;

void initializeLCD()
{
  uint8_t note[8]  = {0x2, 0x3, 0x2, 0xe, 0x1e, 0xc, 0x0};
	lcd.begin();
	lcd.backlight();
  lcd.createChar(1, note);
}

void printHomeScreen()
{
	lcd.clear();
	lcd.setCursor(1, 0);
	lcd.print("Welcome to vik");
	lcd.setCursor(2, 1);
	lcd.print("Player Piano");
  lcd.setCursor(0, 1);
  lcd.write(1);
  lcd.setCursor(15, 1);
  lcd.write(1);
}

void updateDisplay()
{
	lcd.clear();
	switch(menuState)
	{
	case MenuStates::WELCOME:
		printHomeScreen();
		break;
	case MenuStates::SETTINGS:
		lcd.print(MENU_NAMES[currentMenu]);
		lcd.setCursor(0, 1);
		//see if the current menu is true or false or just a number
		if(currentMenu != static_cast<int>(SettingID::SCHEDULE_NOTES) &&
		   currentMenu != static_cast<int>(SettingID::HANDLE_NOTES) &&
			 currentMenu != static_cast<int>(SettingID::WIFI_AP)) // if current menu is not a bool menu
			lcd.print(EEPROM.read(currentMenu));
		else
		{
			if(EEPROM.read(currentMenu))
				lcd.print("True");
			else
				lcd.print("False");
		}
		break;
	case MenuStates::VOLUME:
		lcd.setCursor(0, 0);
		lcd.print("Volume");
		lcd.print("          ");
		lcd.setCursor(0, 1);
		lcd.print(lastAnalog);
		lcd.print("%            ");
		break;
	case MenuStates::RESET:
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("Resetting Keys");
		lcd.setCursor(0, 1);
		lcd.print("...");
		break;
	}
}

void checkSchedule()
{
	unsigned long ms = millis();
	if(ms >= lastPressedOverall + SETTING_MENU_TIMEOUT && lastPressedOverall > 0)
	{
		lastPressedOverall = 0;
		if(menuState == MenuStates::SETTINGS) //if another screen isn't scheduled
		{
			menuState = MenuStates::WELCOME;
			updateDisplay();
		}
	}
	if(ms >= exitScreen && exitScreen > 0)
	{
		exitScreen = 0;
		if(lastPressedOverall == 0) //if setting menu is timed out
			menuState = MenuStates::WELCOME;
		else
			menuState = MenuStates::SETTINGS;
		updateDisplay();
	}
}
