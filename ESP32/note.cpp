#include "note.h"
#include "note.h"
#include "settings.h"
#include "serial.h"

Note notes[88];

int Note::scheduledLeftNotes = 0;
int Note::scheduledRightNotes = 0;
int Note::idGenerator = 0;
int Note::noteVelocityMs[127];

Note::Note()
{
	id = idGenerator;
	idGenerator++;
	//initialize one row of vectors so program won't crash while comparing
	//initialize note as off by default
	for(int index = 0; index < 6; index++)
	{
		schedule[index].reserve(8);
		schedule[index].resize(1);
	}
	schedule[OFF].push_back(millis());
}

void Note::prepareToSchedule(uint8_t velocity)
{
	if(Setting::handleNotes)
	{
		calculateVolume(velocity);
		if(Setting::scheduleNotes)
		{
			if(velocity == 0)
				scheduleNote(velocity);
			else if(canBeScheduled())
				scheduleNote(velocity);
		} else
			sendMidiToProMicro(id, velocity);
	} else
		sendMidiToProMicro(id, velocity); //without modifying the volume
}

void Note::checkSchedule()
{
	//reverse stack behavior: erases stack from the bottom
	unsigned long ms = millis();

	if(schedule[OFF].size() > 1 && schedule[DEACTIVATION].size() > 1 &&
		ms >= schedule[OFF].at(1) && schedule[OFF].at(1) >= schedule[DEACTIVATION].at(1)) //first because sometimes off and on times are the same
	{
		schedule[DEACTIVATION].erase(++schedule[DEACTIVATION].begin());
	}
	if(schedule[STARTUP].size() > 1 && schedule[OFF].size() > 1 && ms >= schedule[STARTUP].at(1) &&
		schedule[STARTUP].at(1) >= schedule[OFF].at(1))
	{
		schedule[OFF].erase(++schedule[OFF].begin());
		sendMidiToProMicro(id, 127);
	}
	if(schedule[ACTIVATION].size() > 1 && schedule[STARTUP].size() > 1 && schedule[VELOCITY].size() > 1 &&
		ms >= schedule[ACTIVATION].at(1) && schedule[ACTIVATION].at(1) >= schedule[STARTUP].at(1))
	{
		schedule[STARTUP].erase(++schedule[STARTUP].begin());
		sendMidiToProMicro(id, schedule[VELOCITY].at(1));
	}
	if(schedule[ON].size() > 1 && schedule[ACTIVATION].size() > 1 &&
		ms >= schedule[ON].at(1) && schedule[ON].at(1) >= schedule[ACTIVATION].at(1))
	{
		schedule[ACTIVATION].erase(++schedule[ACTIVATION].begin());
		schedule[VELOCITY].erase(++schedule[VELOCITY].begin());
		sendMidiToProMicro(id, 127);
	}
	if(schedule[DEACTIVATION].size() > 1 && schedule[ON].size() > 1 &&
		ms >= schedule[DEACTIVATION].at(1) && schedule[DEACTIVATION].at(1) >= schedule[ON].at(1))
	{
		schedule[ON].erase(++schedule[ON].begin());
		sendMidiToProMicro(id, 0);
	}
}

void Note::checkForErrors()
{
	unsigned long ms = millis();
	if(ms >= timeSinceActivation + Setting::noteTimeoutMs && timeSinceActivation > 0) resetSchedule();
	if(schedule[ON].size() > 1 && ms >= schedule[ON].at(1) + Setting::noteTimeoutMs) resetSchedule();
}

void Note::resetSchedule()
{
	if(DEBUG_MODE) Serial.print("Resetting schedule for note: ");
	if(DEBUG_MODE) Serial.println(id);
	for(int index = 0; index < 6; index++)
	{
		schedule[index].resize(1);
		schedule[index].at(0) = 0;
	}
	schedule[OFF].push_back(millis());
	if(timeSinceActivation > 0)
		updateInstance(false);
	timeSinceActivation = 0;
	instances = 0;
	sendMidiToProMicro(id, 0);
}

void Note::resetInstances()
{
	//warning: only call this function in conjunction with resetSchedule()!
	scheduledLeftNotes = 0;
	scheduledRightNotes = 0;
}

void Note::scheduleNote(uint8_t velocity)
{
	if(DEBUG_MODE) sendScheduleToSerial();
	unsigned long ms = millis();
	unsigned long msAndDelay = ms + fullDelay;
	using namespace Setting;

	if(velocity > 0) //if note on command
	{
		int velocityMs = noteVelocityMs[velocity - 1];
		instances++;
		if(instances == 1) //if note is scheduled to deactivate (was 0 before instances++)
		{
			if(msAndDelay - velocityMs - startupMs >= schedule[OFF].back()) //if new note can be scheduled with current scheduling
			{
				schedule[STARTUP].   push_back(msAndDelay - velocityMs - startupMs);
				schedule[ACTIVATION].push_back(msAndDelay - velocityMs);
				schedule[ON].        push_back(msAndDelay);
				schedule[VELOCITY].  push_back(velocity);
				timeSinceActivation == ms;
				updateInstance(true);
			} else if(msAndDelay - deactivateMs - velocityMs - startupMs >= schedule[ON].back()) //if current scheduling can be modified to still schedule the new note
			{
				schedule[DEACTIVATION].push_back(msAndDelay - velocityMs - startupMs - deactivateMs);
				schedule[DEACTIVATION].erase(----schedule[DEACTIVATION].end());
				schedule[OFF].         push_back(msAndDelay - velocityMs - startupMs);
				schedule[OFF].         erase(----schedule[OFF].end());
				schedule[STARTUP].     push_back(msAndDelay - velocityMs - startupMs);
				schedule[ACTIVATION].  push_back(msAndDelay - velocityMs);
				schedule[ON].          push_back(msAndDelay);
				schedule[VELOCITY].    push_back(velocity);
				timeSinceActivation == ms;
				updateInstance(true);
			} else if(msAndDelay - fastDeactivateMs - velocityMs - startupMs >= schedule[ACTIVATION].back()) //if current scheduling can be modified with fast deactivation to schedule the new note
			{
				schedule[ON].          push_back(msAndDelay - velocityMs - startupMs - fastDeactivateMs);
				schedule[ON].          erase(----schedule[ON].end());
				schedule[DEACTIVATION].push_back(msAndDelay - velocityMs - startupMs - fastDeactivateMs);
				schedule[DEACTIVATION].erase(----schedule[DEACTIVATION].end());
				schedule[OFF].         push_back(msAndDelay - velocityMs - startupMs);
				schedule[OFF].         erase(----schedule[OFF].end());
				schedule[STARTUP].     push_back(msAndDelay - velocityMs - startupMs);
				schedule[ACTIVATION].  push_back(msAndDelay - velocityMs);
				schedule[ON].          push_back(msAndDelay);
				schedule[VELOCITY].    push_back(velocity);
				timeSinceActivation == ms;
				updateInstance(true);
			}
		} else //note is scheduled to activate and not deactivate
		{
			if(msAndDelay - deactivateMs - velocityMs - startupMs >= schedule[ON].back()) //if current scheduling can be modified to still schedule the new note
			{
				schedule[DEACTIVATION].push_back(msAndDelay - velocityMs - startupMs - deactivateMs);
				schedule[OFF].         push_back(msAndDelay - velocityMs - startupMs);
				schedule[STARTUP].     push_back(msAndDelay - velocityMs - startupMs);
				schedule[ACTIVATION].  push_back(msAndDelay - velocityMs);
				schedule[ON].          push_back(msAndDelay);
				schedule[VELOCITY].    push_back(velocity);
			} else if(msAndDelay - fastDeactivateMs - velocityMs - startupMs >= schedule[ACTIVATION].back() && schedule[ACTIVATION].back() > 0) //if current scheduling can be modified with fast deactivation to still schedule the new note
			{
				schedule[ON].          push_back(msAndDelay - velocityMs - startupMs - fastDeactivateMs);
				schedule[ON].          erase(----schedule[ON].end());
				schedule[DEACTIVATION].push_back(msAndDelay - velocityMs - startupMs - fastDeactivateMs);
				schedule[OFF].         push_back(msAndDelay - velocityMs - startupMs);
				schedule[STARTUP].     push_back(msAndDelay - velocityMs - startupMs);
				schedule[ACTIVATION].  push_back(msAndDelay - velocityMs);
				schedule[ON].          push_back(msAndDelay);
				schedule[VELOCITY].    push_back(velocity);
			}
		}
	} else if(instances > 0 /*&& velocity == 0*/) //if note off command and note is not already off
	{
		if(instances > 1) //if this isn't the last instance
		{
			//remove instance and exit
			instances--;
		} else //this is the last instance of the note and it should be scheduled
		{
			instances = 0;
			timeSinceActivation == 0;
			updateInstance(false);

			if(msAndDelay - fastDeactivateMs >= schedule[ACTIVATION].back() && msAndDelay - fastDeactivateMs <= schedule[ON].back() && schedule[ACTIVATION].back() > 0) //if it's efficient to use fast deactivation
			{
				schedule[ON].          push_back(msAndDelay - fastDeactivateMs);
				schedule[ON].          erase(----schedule[ON].end());
				schedule[DEACTIVATION].push_back(msAndDelay - fastDeactivateMs);
				schedule[OFF].         push_back(msAndDelay);
			} else if(msAndDelay - deactivateMs >= schedule[ON].back()) //if regular deactivation works
			{
				schedule[DEACTIVATION].push_back(msAndDelay - deactivateMs);
				schedule[OFF].         push_back(msAndDelay);
			} else //if all else fails the key shouldn't stay stuck on
			{
				if(schedule[ACTIVATION].back() > 0)
				{
					//immediately deactivate the key as soon as it makes sound
					schedule[ON].          push_back(schedule[ACTIVATION].back());
					schedule[ON].          erase(----schedule[ON].end());
					schedule[DEACTIVATION].push_back(schedule[ACTIVATION].back());
					schedule[OFF].         push_back(schedule[ACTIVATION].back() + fastDeactivateMs);
				} else //this should never happen
				{
					schedule[DEACTIVATION].push_back(msAndDelay);
					schedule[OFF].         push_back(msAndDelay + deactivateMs);
				}
			}
		}
	}
	if(DEBUG_MODE) sendScheduleToSerial();
}

void Note::calculateVolume(uint8_t& velocity)
{
	if(velocity > 0)
	{
		velocity = round((velocity * Setting::volume) / (double)100);
		if(velocity > 127)
			velocity = 127;
		else if(velocity < 1)
			velocity = 1;
	}
}

void Note::updateInstance(boolean state)
{
	if(id < 44)
	{
		if(state)
			scheduledLeftNotes++;
		else
			scheduledLeftNotes--;
	} else
	{
		if(state)
			scheduledRightNotes++;
		else
			scheduledRightNotes--;
	}
}

void Note::sendScheduleToSerial()
{
	Serial.println("-----------------------");
	Serial.print("Schedule for note: ");
	Serial.print(id);
	Serial.print(" At the time: ");
	Serial.println(millis());
	for(int i = 0; i < 6; i++)
	{
		for(int j = 0; j < schedule[i].size(); j++)
		{
			Serial.print(schedule[i].at(j), DEC);
			Serial.print(" ");
		}
		Serial.println(" ");
		Serial.print(" ");
	}
}

bool Note::canBeScheduled()
{
	if(id < 44)
	{
		if(scheduledLeftNotes < Setting::maxLeftNotes)
			return true;
	} else
		if(id >= 44)
		{
			if(scheduledRightNotes < Setting::maxRightNotes)
				return true;
		}
	return false;
}