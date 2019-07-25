#include <EEPROM.h>
#include "input.h"
#include "lcd.h"
#include "serial.h"
#include "setting.h"

void handleInput(int inputPin);

extern const int UP_PIN         = 4;
extern const int DOWN_PIN       = 6;
extern const int LEFT_PIN       = 7;
extern const int RIGHT_PIN      = 5;
extern const int RESET_PIN      = 8;
extern const int VOLUME_PIN     = 10;

struct LastPressed
{
	unsigned long up     = 0;
	unsigned long down   = 0;
	unsigned long left   = 0;
	unsigned long right  = 0;
	unsigned long reset1 = 0;
}; LastPressed lastPressed;
unsigned long  lastPressedOverall = 0;
int            lastAnalog;

void initializeInputs()
{
	pinMode(UP_PIN, INPUT);
	pinMode(DOWN_PIN, INPUT);
	pinMode(LEFT_PIN, INPUT);
	pinMode(RIGHT_PIN, INPUT);
	pinMode(RESET_PIN, INPUT);
	lastAnalog = (analogRead(VOLUME_PIN) / 20);
}

void checkForInput()
{
	unsigned long ms = millis();
	const int BUTTON_COOLDOWN = 500;

	if(digitalRead(UP_PIN) == HIGH && lastPressed.up + BUTTON_COOLDOWN <= ms)
		handleInput(UP_PIN);
	else if(digitalRead(DOWN_PIN) == HIGH && lastPressed.down + BUTTON_COOLDOWN <= ms)
		handleInput(DOWN_PIN);
	else if(digitalRead(LEFT_PIN) == HIGH && lastPressed.left + BUTTON_COOLDOWN <= ms)
		handleInput(LEFT_PIN);
	else if(digitalRead(RIGHT_PIN) == HIGH && lastPressed.right + BUTTON_COOLDOWN <= ms)
		handleInput(RIGHT_PIN);
	else if(digitalRead(RESET_PIN) == HIGH && lastPressed.reset1 + BUTTON_COOLDOWN <= ms)
		handleInput(RESET_PIN);
	else
	{
		const int ANALOG_CHANGE_RATE = 5;
		const int ANALOG_WITHIN_RANGE = 15;
		const int ANALOG_CHANGE_TIME = BUTTON_COOLDOWN * 2;
		int analogValue = analogRead(VOLUME_PIN) / 20; //conform analog value from 0-200
		if((analogValue >= (lastAnalog + ANALOG_CHANGE_RATE) || analogValue <= (lastAnalog - ANALOG_CHANGE_RATE)) && //if volume has changed enough
			(analogValue < (lastAnalog + ANALOG_WITHIN_RANGE) && analogValue >(lastAnalog - ANALOG_WITHIN_RANGE)) && //if volume hasn't changed too much for it to be a glitch
			lastPressedOverall + ANALOG_CHANGE_TIME <= ms)                        //if another button hasn't been pressed within a certain period for it to affect the reading
		{
			//conform analog to certain values if they are within range
			if(analogValue <= 105 && analogValue >= 95)
				analogValue = 100;
			else if(analogValue > 195)
				analogValue = 200;
			else if(analogValue < 5)
				analogValue = 0;
			lastAnalog = analogValue;
			handleInput(VOLUME_PIN); //implied that lastAnalog has also been changed
		}
	}
}

void handleInput(int inputPin)
{
	unsigned long ms = millis();
	switch(inputPin)
	{
	case UP_PIN:
		if(menuState == MenuStates::SETTINGS) //prevent settings from changing while not showing
		{
			lastPressed.up = ms;
			lastPressedOverall = ms;
			changeSetting(1);
			sendSerialToMain(SerialConstants::SETTING_HEADER, currentMenu, EEPROM.read(currentMenu));
			updateDisplay();
		}
		break;
	case DOWN_PIN:
		if(menuState == MenuStates::SETTINGS) //prevent settings from changing while not showing
		{
			lastPressed.down = ms;
			lastPressedOverall = ms;
			changeSetting(-1);
			sendSerialToMain(SerialConstants::SETTING_HEADER, currentMenu, EEPROM.read(currentMenu));
			updateDisplay();
		}
		break;
	case LEFT_PIN:
		lastPressed.left = ms;
		lastPressedOverall = ms;
		currentMenu--;
		if(currentMenu < 0)
			currentMenu = NUM_OF_MENUS - 1;
		menuState = MenuStates::SETTINGS;
		updateDisplay();
		break;
	case RIGHT_PIN:
		lastPressed.right = ms;
		lastPressedOverall = ms;
		currentMenu++;
		if(currentMenu >= NUM_OF_MENUS)
			currentMenu == 0;
		menuState = MenuStates::SETTINGS;
		updateDisplay();
		break;
	case RESET_PIN:
		lastPressed.reset1 = ms;
		exitScreen = ms + SPECIAL_MENU_TIMEOUT;
		sendSerialToMain(SerialConstants::RESET_HEADER, 127, 127);
		menuState = MenuStates::RESET;
		updateDisplay();
		break;
	case VOLUME_PIN:
		exitScreen = ms + SPECIAL_MENU_TIMEOUT;
		menuState = MenuStates::VOLUME;
		sendSerialToMain(SerialConstants::VOLUME_HEADER, lastAnalog, lastAnalog);
		updateDisplay();
		break;
	}
}
