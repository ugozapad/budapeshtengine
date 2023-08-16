#ifndef TIMER_H
#define TIMER_H

//! Base class for system timer
class SystemTimer
{
public:
	//! Initialization 
	virtual void Init() = 0;

	//! Shutdown
	virtual void Shutdown() = 0;

	//! Update timer
	virtual void Update() = 0;

	//! Get time from application start
	virtual float GetTime() = 0;

	//! Get delta
	virtual float GetDelta() = 0;
};

// dont want to use global var for that :(
ENGINE_API SystemTimer* GetSystemTimer();

#endif // !TIMER_H
