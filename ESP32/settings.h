#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

namespace SettingID //so a using statement can be used in the switch statement
{
	enum SettingID
	{
		HANDLE_NOTES,
		SCHEDULE_NOTES,
		MIN_ACCEPTED_VEL,
		PWM_PERCENT,
		MIN_STARTUP_MS,
		MAX_STARTUP_MS,
		VELOCITY_VAR,
		MIN_DEACTIVATE_MS,
		MAX_DEACTIVATE_MS,
		FAST_DEACTIVATE_MS,
		SUSTAIN_ON_MS,
		SUSTAIN_OFF_MS,
		NOTE_TIMEOUT_MS,
		SUSTAIN_TIMEOUT_MS,
		AUTO_RESET_MS,
		MAX_LEFT_NOTES,
		MAX_RIGHT_NOTES
	};
}

extern int           fullDelay;
extern bool       acceptMidi;
extern unsigned long nextReset;

namespace Setting
{
	extern bool handleNotes;
	extern bool scheduleNotes;
	extern int     minNoteVelocity;
	extern int     minSolenoidPWM;
	extern int     minStartupMs;
	extern int     maxStartupMs;
	extern int     velocityVar;
	extern int     minDeactivateMs;
	extern int     maxDeactivateMs;
	extern int     fastDeactivateMs;
	extern int     sustainOnMs;
	extern int     sustainOffMs;
	extern int     noteTimeoutMs;
	extern int     sustainTimeoutMs;
	extern int     autoResetMs;
	extern int     maxLeftNotes;
	extern int     maxRightNotes;
	extern int     volume;
}

void setVolume(int newVolume);
void updateSetting(SettingID::SettingID setting, int value);
void msReset();

#endif