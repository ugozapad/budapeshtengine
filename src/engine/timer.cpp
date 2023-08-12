#include "pch.h"
#include "timer.h"

#if defined(WIN32) || defined(_WIN32)
class SystemTimerWin32 : public SystemTimer
{
public:
	void init();
	void shutdown();

	void update();

	float getTime();
	float getDelta();

private:
	LARGE_INTEGER m_frequency;
	LARGE_INTEGER m_startTime;
	LARGE_INTEGER m_endTime;

	float m_floatFrequency;
	float m_deltaTime;
};

void SystemTimerWin32::init()
{
	QueryPerformanceFrequency(&m_frequency);
	m_floatFrequency = (float)m_frequency.QuadPart;

	QueryPerformanceCounter(&m_startTime);
	m_startTime = m_endTime;
}

void SystemTimerWin32::shutdown()
{
}

void SystemTimerWin32::update()
{
	QueryPerformanceCounter(&m_startTime);
	m_deltaTime = static_cast<float>(m_startTime.QuadPart - m_endTime.QuadPart) / m_frequency.QuadPart;
	m_endTime = m_startTime;
}

float SystemTimerWin32::getTime()
{
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	return static_cast<float>(time.QuadPart / m_frequency.QuadPart);
}

float SystemTimerWin32::getDelta()
{
	return m_deltaTime;
}

SystemTimer* getSystemTimer()
{
	static SystemTimerWin32 instance;
	return &instance;
}
#endif // WIN32