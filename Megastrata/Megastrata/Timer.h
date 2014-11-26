#pragma once

#include <utils.h>

class Timer
{
public:
	Timer(void);

	void TogglePause();

	ElapsedTime TimeElapsed();

	ElapsedTime GetAbsoluteTime();

private:
	ElapsedTime last_time_measured;
	ElapsedTime ticks_per_ms;

	ElapsedTime absolute_time;

	bool m_timepaused;

};