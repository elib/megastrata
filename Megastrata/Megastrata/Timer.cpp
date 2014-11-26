#include "Timer.h"

//WINDOWS VERSION.

#include "windows.h"

Timer::Timer(void)
{
	//get time now, place in class members
	LARGE_INTEGER timenow, tickssecond;
	QueryPerformanceFrequency(&tickssecond);
	QueryPerformanceCounter(&timenow);
	
	last_time_measured = timenow.QuadPart;
	ticks_per_ms = tickssecond.QuadPart / 1000;

	FILETIME filetimenow;
	SYSTEMTIME systemtime;
	GetSystemTime(&systemtime);
	SystemTimeToFileTime(&systemtime, &filetimenow);
	ULARGE_INTEGER systimenow;
	memcpy(&systimenow, &filetimenow, sizeof(filetimenow));

	SYSTEMTIME firsttick;
	firsttick.wMilliseconds = 1;
	firsttick.wSecond = 1;
	firsttick.wMinute = 1;
	firsttick.wHour = 1;
	firsttick.wDay = 1;
	firsttick.wDayOfWeek = 1;
	firsttick.wMonth = 1;
	firsttick.wYear = 2000; //since the year 2000
	FILETIME firsttickfiletime;
	SystemTimeToFileTime(&firsttick, &firsttickfiletime);

	ULARGE_INTEGER firstticktime;
	memcpy(&firstticktime, &firsttickfiletime, sizeof(firsttickfiletime));
	
	__int64 time_since = systimenow.QuadPart - firstticktime.QuadPart;
	absolute_time = time_since / 10000;

	m_timepaused = false;

}

__int64 Timer::TimeElapsed()
{
	//get difference in counter
	LARGE_INTEGER timenow;
	QueryPerformanceCounter(&timenow);
	__int64 diff = timenow.QuadPart - last_time_measured;

	//set new time
	last_time_measured = timenow.QuadPart;
	__int64 ms_elapsed = diff / ticks_per_ms;
	
	if(!m_timepaused)
		absolute_time += ms_elapsed;

	return ms_elapsed;
}


ElapsedTime Timer::GetAbsoluteTime()
{
	return absolute_time;
}

void Timer::TogglePause()
{
	m_timepaused = !m_timepaused;
}