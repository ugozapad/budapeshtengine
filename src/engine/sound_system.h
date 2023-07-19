#ifndef SOUND_SYSTEM_H
#define SOUND_SYSTEM_H
#pragma once

class ISound
{
public:
	virtual			~ISound		() { }

	virtual void	play		() = 0;
	virtual void	stop		() = 0;
	virtual void	pause		() = 0;

	virtual bool	isPlaying	() const		= 0;
	virtual bool	isPaused	() const		= 0;

	virtual bool	isLooped	() const		= 0;
	virtual void	setLooped	(bool bLooped)	= 0;

	virtual float	volume		() const		= 0;
	virtual void	setVolume	(float fVolume) = 0;

	virtual void	onFrame		(float delta)	= 0;
};

typedef unsigned int SoundDeviceID;

class IAllocator;
class ISoundSystem
{
public:
	static ISoundSystem* create(const char* sSoundSystemName);

public:
	virtual					~ISoundSystem	() { }

	virtual void			selectDevice	(SoundDeviceID deviceId)	= 0;
	virtual SoundDeviceID	selectedDevice	() const					= 0;

	virtual ISound*			createSound		(const char* sSoundFile)	= 0;
	virtual void			destroySound	(ISound*& pSound)			= 0;

	virtual void			onFrame			(float delta)				= 0;

	virtual void			stopAllSounds	()							= 0;
	virtual void			pauseAllSounds	()							= 0;
	virtual	void			releaseAllSounds()							= 0;

	virtual float			getMasterVolume	() const					= 0;
	virtual void			setMasterVolume	(float fVolume)				= 0;
};

extern ISoundSystem* g_pSoundSystem;

#ifndef ENGINE_EXPORTS
extern "C"
{
	__declspec(dllexport) ISoundSystem* __stdcall createSoundSystem();
};
#else
typedef ISoundSystem*(__stdcall *createSoundSystem_t)();
#endif

#endif
