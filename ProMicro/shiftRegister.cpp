//Latchpin: Pin 18 (A0) goes to second register input
//Datapin:  Pin 16 goes to first register input
//Clockpin: Pin 15 goes to third register input
const int ShiftPWM_latchPin=18; //values assigned before includes
const bool ShiftPWM_invertOutputs = false;
const bool ShiftPWM_balanceLoad = false;
#include <ShiftPWM.h>
#include <CShiftPWM.h>
#include "shiftRegister.h"

void conformVelocity(uint8_t* velocity);

extern uint8_t pwmPercent = 45;
const char     MAX_PWM = 235;

void intitializeRegisters()
{
	const char    PWM_FREQUENCY = 75;
	const int     NUM_REGISTERS = 11;
	ShiftPWM.SetAmountOfRegisters(NUM_REGISTERS);
	ShiftPWM.Start(PWM_FREQUENCY, MAX_PWM);
}

void activateNote(uint8_t note, uint8_t velocity)
{
	if(velocity > 0)
		conformVelocity(&velocity);
	ShiftPWM.SetOne(note, velocity);
}

void conformVelocity(uint8_t* velocity)
{
	//conforms velocity from 0-127 to 0-255 while taking into account the minimum possible solenoid PWM
	const double MIN_PWM = round(MAX_PWM * pwmPercent / static_cast<double>(100));
	*velocity = round((*velocity) * (MAX_PWM - MIN_PWM) / static_cast<double>(127) + MIN_PWM);
}

void testRegisters()
{
	for(int led = 0; led < 88; led++)
	{
		ShiftPWM.SetOne(led, MAX_PWM);
		delay(50);
		ShiftPWM.SetOne(led, 0);
	}
}


