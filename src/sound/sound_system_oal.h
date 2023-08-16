#ifndef SOUND_SYSTEM_OAL_H
#define SOUND_SYSTEM_OAL_H
#pragma once

#define AL_GENERIC_HARDWARE "Generic Hardware"
#define AL_GENERIC_SOFTWARE "Generic Software"

typedef ALCdevice	SoundDevice;
typedef ALCcontext	SoundDeviceContext;

class SoundSystem_OpenAL : public ISoundSystem
{
	enum SoundDeviceFeature_
	{
		// EAX flags
		SoundDeviceFeature_EAXSupported	= (1 << 0),
		SoundDeviceFeature_EAX20		= (1 << 1),
		SoundDeviceFeature_EAX30		= (1 << 2),
		SoundDeviceFeature_EAX40		= (1 << 3),
		SoundDeviceFeature_EAX50		= (1 << 4),
		// EFX flags
		SoundDeviceFeature_EFX			= (1 << 5),
		// XRAM flags
		SoundDeviceFeature_XRAM			= (1 << 6)
	};
	typedef unsigned char SoundDeviceFeatures;
	struct SoundDeviceDescr
	{
		struct
		{
			int iMajor;
			int iMinor;
		} version;

		SoundDeviceFeatures flags;
		const char*			name;
		const char*			real_name;
	};
	typedef Array<SoundDeviceDescr> DEVICES;
	typedef Array<ISound*>			SOUNDS;
public:
	SoundSystem_OpenAL	();
	~SoundSystem_OpenAL	();

	void			selectDevice	(SoundDeviceID deviceId) override;
	SoundDeviceID	selectedDevice	() const override;

	ISound* createSound	(const char* sSoundFile) override;
	void	destroySound(ISound*& pSound) override;

	void	Update		(float delta) override;


	void	stopAllSounds	() override;
	void	pauseAllSounds	() override;
	void	releaseAllSounds() override;
	

	float	getMasterVolume	() const override;
	void	setMasterVolume	(float fVolume) override;

private:
	DEVICES		m_devices;
	SOUNDS		m_sounds;
};

#endif
