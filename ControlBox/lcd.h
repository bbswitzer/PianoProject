#ifndef LCD_H
#define LCD_H

#include <Arduino.h>

enum class MenuStates
{
	WELCOME,
	SETTINGS,
	VOLUME,
	RESET
}; extern MenuStates menuState;

extern const String  MENU_NAMES[];
extern const int     SPECIAL_MENU_TIMEOUT;
extern const int     NUM_OF_MENUS;
extern int           currentMenu;
extern unsigned long exitScreen;

void initializeLCD();
void printHomeScreen();
void updateDisplay();
void checkSchedule();

#endif