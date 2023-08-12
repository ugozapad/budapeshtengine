#ifndef TIMER_H
#define TIMER_H

//! Base class for system timer
class SystemTimer
{
public:
	//! Initialization 
	virtual void init() = 0;

	//! Shutdown
	virtual void shutdown() = 0;

	//! Update timer
	virtual void update() = 0;

	//! Get time from application start
	virtual float getTime() = 0;

	//! Get delta
	virtual float getDelta() = 0;
};

// dont want to use global var for that :(
ENGINE_API SystemTimer* getSystemTimer();

#endif // !TIMER_H
