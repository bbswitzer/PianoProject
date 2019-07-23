#include "sustain.h"
#include "sustain.h"
#include "serial.h"
#include "settings.h"

Sustain sustain;

Sustain::Sustain()
{
	//initialize one row of vectors so program won't crash while comparing
	//initialize sustain as off by default
	for(int index = 0; index < 4; index++)
	{
		schedule[index].reserve(6);
		schedule[index].resize(1);
	}
	schedule[OFF].push_back(millis());
}

void Sustain::prepareToSchedule(uint8_t velocity)
{
	const uint8_t SUSTAIN_MIN_VELOCITY = 64;
	if(Setting::handleNotes && Setting::scheduleNotes)
	{
		if(velocity < SUSTAIN_MIN_VELOCITY)
		{
			scheduleSustain(false);
		} else if(instances == 0) //only schedule if sustain is off
			scheduleSustain(true);
	} else
	{
		if(velocity < SUSTAIN_MIN_VELOCITY)
			ledcWrite(0, 0);
		else
			ledcWrite(0, 255);
	}
}

void Sustain::checkSchedule()
{
	unsigned long ms = millis();
	//less checks for sustain because it's slower and less important
	if(schedule[OFF].size() > 1 && schedule[DEACTIVATION].size() > 1 &&
		ms >= schedule[OFF].at(1) && schedule[OFF].at(1) >= schedule[DEACTIVATION].at(1))
	{
		schedule[DEACTIVATION].erase(schedule[DEACTIVATION].begin()++);
		ledcWrite(0, 0);
	}
	if(schedule[ACTIVATION].size() > 1 && schedule[OFF].size() > 1 &&
		ms >= schedule[ACTIVATION].at(1) && schedule[ACTIVATION].at(1) >= schedule[OFF].at(1))
	{
		schedule[OFF].erase(schedule[OFF].begin()++);
		ledcWrite(0, 255);
	}
	if(schedule[ON].size() > 1 && schedule[ACTIVATION].size() > 1 &&
		ms >= schedule[ON].at(1) && schedule[ON].at(1) >= schedule[ACTIVATION].at(1))
	{
		schedule[ACTIVATION].erase(schedule[ACTIVATION].begin()++);
	}
	if(schedule[DEACTIVATION].size() > 1 && schedule[ON].size() > 1 &&
		ms >= schedule[DEACTIVATION].at(1) && schedule[DEACTIVATION].at(1) >= schedule[ON].at(1))
	{
		schedule[ON].erase(schedule[ON].begin()++);
		ledcWrite(0, 30);
	}
}

void Sustain::checkForErrors()
{
	unsigned long ms = millis();
	if(ms >= timeSinceActivation + Setting::sustainTimeoutMs && timeSinceActivation > 0) resetSchedule();
	if(schedule[ON].size() > 1) if(ms >= schedule[ON].at(1) + Setting::sustainTimeoutMs) resetSchedule();
}

void Sustain::resetSchedule()
{
	for(int index = 0; index < 4; index++)
		schedule[index].resize(1);
	schedule[OFF].push_back(millis());
	timeSinceActivation = 0;
	instances = 0;
	ledcWrite(0, 0);
}

void Sustain::scheduleSustain(boolean state)
{
	unsigned long ms = millis();
	unsigned long msAndDelay = ms + fullDelay;
	using namespace Setting;

	if(state)
	{
		instances++;
		if(msAndDelay - sustainOnMs >= schedule[OFF].back()) //if sustain can be scheduled with current scheduling
		{
			schedule[ACTIVATION].push_back(msAndDelay - sustainOnMs);
			schedule[ON].        push_back(msAndDelay);
			timeSinceActivation = ms;
		} else
			if(msAndDelay - sustainOffMs - sustainOnMs >= schedule[ON].back()) //if current scheduling can be modified to still schedule the sustain
			{
				schedule[ON].        push_back(msAndDelay - sustainOnMs - sustainOffMs);
				schedule[ON].        erase(----schedule[ON].end());
				schedule[OFF].       push_back(msAndDelay - sustainOnMs);
				schedule[OFF].       erase(----schedule[OFF].end());
				schedule[ACTIVATION].push_back(msAndDelay - sustainOnMs);
				schedule[ON].        push_back(msAndDelay);
				timeSinceActivation = ms;
			}
	} else
		if(instances > 0) //if sustain off command and sustain is not already off
		{
			instances == 0;
			if(msAndDelay - sustainOffMs >= schedule[ON].back()) //if sustain can be ideally deactivated
			{
				schedule[DEACTIVATION].push_back(msAndDelay - sustainOffMs);
				schedule[OFF].         push_back(msAndDelay);
			} else //deactivate sustain anyways so it's not stuck on
			{
				schedule[DEACTIVATION].push_back(msAndDelay);
				schedule[OFF].         push_back(msAndDelay + sustainOffMs);
			}
		}
}