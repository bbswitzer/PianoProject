#include "settings.h"
#include "settings.h"
#include "note.h"
#include "serial.h"

//settings within the program
int           fullDelay      = 0;
bool       acceptMidi     = false;
unsigned long nextReset      = 0; //first reset will happen immediately

//settings changed by the control box
namespace Setting
{
	bool handleNotes      = true;
	bool scheduleNotes    = true;
	int     minNoteVelocity  = 4;
	int     minStartupMs     = 18;
	int     maxStartupMs     = 18;
	int     velocityVar      = 35;
	int     minDeactivateMs  = 75;
	int     maxDeactivateMs  = 80;
	int     fastDeactivateMs = 52;
	int     sustainOnMs      = 91;
	int     sustainOffMs     = 50;
	int     noteTimeoutMs    = 10000;
	int     sustainTimeoutMs = 30000;
	int     autoResetMs      = 360000;
	int     maxLeftNotes     = 26;
	int     maxRightNotes    = 22;
	int     volume           = 100;
}

void setVolume(int newVolume) { Setting::volume = newVolume; }

void updateSetting(SettingID::SettingID setting, int value)
{
	using namespace SettingID;
	using namespace Setting;
	switch(setting)
	{
	case HANDLE_NOTES:
		handleNotes = value;
		break;
	case SCHEDULE_NOTES:
		scheduleNotes = value;
		break;
	case MIN_ACCEPTED_VEL:
		minNoteVelocity = value;
		break;
	case PWM_PERCENT:
		//only setting passed to pro micro
		extern const byte SETTING_HEADER;
		customSerialToProMicro(SETTING_HEADER, value, value);
		break;
	case MIN_STARTUP_MS:
		minStartupMs = value;
		msReset();
		break;
	case MAX_STARTUP_MS:
		maxStartupMs = value;
		msReset();
		break;
	case VELOCITY_VAR:
		velocityVar = value;
		msReset();
		break;
	case MIN_DEACTIVATE_MS:
		minDeactivateMs = value;
		msReset();
		break;
	case MAX_DEACTIVATE_MS:
		maxDeactivateMs = value;
		msReset();
		break;
	case FAST_DEACTIVATE_MS:
		fastDeactivateMs = value;
		break;
	case SUSTAIN_ON_MS:
		sustainOnMs = value;
		break;
	case SUSTAIN_OFF_MS:
		sustainOffMs = value;
		break;
	case NOTE_TIMEOUT_MS:
		noteTimeoutMs = value * 1000;
		break;
	case SUSTAIN_TIMEOUT_MS:
		sustainTimeoutMs = value * 1000;
		break;
	case AUTO_RESET_MS:
		autoResetMs = value * 60000;
		break;
	case MAX_LEFT_NOTES:
		maxLeftNotes = value;
		break;
	case MAX_RIGHT_NOTES:
		maxRightNotes = value;
		break;
	}
}

void msReset()
{
	using namespace Setting;
	for(int noteIndex = 0; noteIndex < 88; noteIndex++)
	{
		//calculate the ms for each key linearly. Calculate this now to process less later
		notes[noteIndex].setStartupMs((noteIndex * (minStartupMs - maxStartupMs) / 88) + maxStartupMs);
		notes[noteIndex].setDeactivateMs((noteIndex * (maxDeactivateMs - minDeactivateMs) / 88) + minDeactivateMs);
	}
	for(int velocityIndex = 0; velocityIndex < 127; velocityIndex++)
	{
		//function created through graphing velocity times and creating a function that best fit
		Note::setNoteVelocityMs(velocityIndex, round(((-25 * velocityIndex) / (double)127) + velocityVar));
	}

	//calculate the total maximum time for a note cycle as a reference when scheduling keys
	fullDelay = maxStartupMs + Note::getNoteVelocityMs(0) + maxDeactivateMs;
}


