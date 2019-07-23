#ifndef NOTE_H
#define NOTE_H

class Note
{
private:
	typedef std::vector<unsigned long> scheduleV_t;
	enum ScheduleID
	{
		STARTUP,
		ACTIVATION,
		VELOCITY,
		ON,
		DEACTIVATION,
		OFF
	};
	scheduleV_t schedule[6];
	int id = 0;
	int startupMs = Setting::maxStartupMs;
	int deactivateMs = Setting::maxDeactivateMs;
	int instances = 0;
	static int scheduledLeftNotes;
	static int scheduledRightNotes;
	static int idGenerator;
	static int noteVelocityMs[127];
	unsigned long timeSinceActivation = 0;

	void scheduleNote(uint8_t velocity);
	void calculateVolume(uint8_t& volume);
	void updateInstance(boolean state);
	void sendScheduleToSerial();
	bool canBeScheduled();
public:
	Note();
	void setStartupMs(int ms) { startupMs = ms; }
	void setDeactivateMs(int ms) { deactivateMs = ms; }
	void prepareToSchedule(uint8_t velocity);
	void checkSchedule();
	void checkForErrors();
	void resetSchedule();
	static void resetInstances();
	static void setNoteVelocityMs(int velocity, int ms) { noteVelocityMs[velocity] = ms; }
	static int  getNoteVelocityMs(int velocity) { return noteVelocityMs[velocity]; }
};

extern Note notes[88];

#endif