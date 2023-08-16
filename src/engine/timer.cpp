#include "pch.h"
#include "timer.h"

#if defined(WIN32) || defined(_WIN32)
class SystemTimerWin32 : public SystemTimer
{
public:
	void Init();
	void Shutdown();

	void Update();

	float GetTime();
	float GetDelta();

private:
	LARGE_INTEGER m_frequency;
	LARGE_INTEGER m_startTime;
	LARGE_INTEGER m_endTime;

	float m_floatFrequency;
	float m_deltaTime;
};

void SystemTimerWin32::Init()
{
	QueryPerformanceFrequency(&m_frequency);
	m_floatFrequency = (float)m_frequency.QuadPart;

	QueryPerformanceCounter(&m_startTime);
	m_startTime = m_endTime;
}

void SystemTimerWin32::Shutdown()
{
}

void SystemTimerWin32::Update()
{
	QueryPerformanceCounter(&m_startTime);
	m_deltaTime = static_cast<float>(m_startTime.QuadPart - m_endTime.QuadPart) / m_frequency.QuadPart;
	m_endTime = m_startTime;
}

float SystemTimerWin32::GetTime()
{
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	return static_cast<float>(time.QuadPart / m_frequency.QuadPart);
}

float SystemTimerWin32::GetDelta()
{
	return m_deltaTime;
}

SystemTimer* GetSystemTimer()
{
	static SystemTimerWin32 instance;
	return &instance;
}
#endif // WIN32