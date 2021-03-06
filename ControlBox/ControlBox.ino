                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          #include <EEPROM.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <MIDIUSB.h>
#include "input.h"
#include "lcd.h"
#include "setting.h"
#include "midi.h"
#include "serial.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);//set the LCD address to 0x27 for a 16 chars and 2 line display

extern const bool DEBUG_MODE = true;

void setup()
{
  initializeInputs();
    initializeLCD();
    printHomeScreen();
    if(DEBUG_MODE) Serial.begin(38400);
    Serial1.begin(38400);

    //send all data to ESP32 for initializationv
    sendAllSettings();
}

void loop()
{
    checkForInput();
    checkSchedule();
    checkForMidiUSB();
}
