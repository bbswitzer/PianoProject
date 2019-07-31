#ifndef SUSTAIN_H
#define SUSTAIN_H

#include <Arduino.h>
#include <vector>

class Sustain
{
private:
	typedef std::vector<unsigned long> scheduleV_t;
	enum ScheduleID
	{
		ACTIVATION,
		ON,
		DEACTIVATION,
		OFF
	};
	scheduleV_t schedule[4];
	int instances = 0;
	unsigned long timeSinceActivation = 0;

	void scheduleSustain(bool state);
public:
	Sustain();
	void prepareToSchedule(uint8_t velocity);
	void checkSchedule();
	void checkForErrors();
	void resetSchedule();
};

extern Sustain sustain;

#endif