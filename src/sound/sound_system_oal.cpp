#include "pch.h"
#include "sound_system_oal.h"
#include <algorithm>

extern "C"
{
	__declspec(dllexport) ISoundSystem* __stdcall createSoundSystem(IAllocator* allocator)
	{
		return (MEM_NEW(*allocator, SoundSystem_OpenAL, allocator));
	}
};

SoundSystem_OpenAL::SoundSystem_OpenAL(IAllocator* allocator)
	: m_pAllocator(allocator), m_devices(*allocator), m_sounds(*allocator)
{
	SoundDeviceID defaultDeviceId = 0;

	if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT"))
	{
		const char* pDevices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);

		while (*pDevices)
		{
			SoundDevice* pDevice = alcOpenDevice(pDevices);
			if (pDevice)
			{
				SoundDeviceContext* pContext = alcCreateContext(pDevice, NULL);
				if (pContext)
				{
					alcMakeContextCurrent(pContext);
					const char* sDeviceName = alcGetString(pDevice, ALC_DEVICE_SPECIFIER);

					if (sDeviceName && *sDeviceName)
					{
						m_devices.push_back(SoundDeviceDescr());
						SoundDeviceDescr& sdd = m_devices.back();

						alcGetIntegerv(pDevice, ALC_MAJOR_VERSION, sizeof(int), &(sdd.version.iMajor));
						alcGetIntegerv(pDevice, ALC_MINOR_VERSION, sizeof(int), &(sdd.version.iMinor));

						sdd.flags = 0;

						if (alIsExtensionPresent("EAX2.0")) sdd.flags |= SoundDeviceFeature_EAX20;
						if (alIsExtensionPresent("EAX3.0")) sdd.flags |= SoundDeviceFeature_EAX30;
						if (alIsExtensionPresent("EAX4.0")) sdd.flags |= SoundDeviceFeature_EAX40;
						if (alIsExtensionPresent("EAX5.0")) sdd.flags |= SoundDeviceFeature_EAX50;
						if (sdd.flags & (SoundDeviceFeature_EAX20 | SoundDeviceFeature_EAX30 | SoundDeviceFeature_EAX40 | SoundDeviceFeature_EAX50))
							sdd.flags |= SoundDeviceFeature_EAXSupported;

						if (alIsExtensionPresent("ALC_EXT_EFX"))
							sdd.flags |= SoundDeviceFeature_EFX;
						if (alIsExtensionPresent("EAX_RAM"))
							sdd.flags |= SoundDeviceFeature_XRAM;

						if (!_stricmp(sDeviceName, AL_GENERIC_HARDWARE) || !_stricmp(sDeviceName, AL_GENERIC_SOFTWARE))
						{
							sdd.flags &= ~SoundDeviceFeature_EAXSupported;
							defaultDeviceId = SoundDeviceID(m_devices.size() - 1);
						}

						sdd.name		= _strdup(pDevices);
						sdd.real_name	= _strdup(sDeviceName);
					}

					alcMakeContextCurrent(NULL);
					alcDestroyContext(pContext);
				}
				alcCloseDevice(pDevice);
			}
			pDevices += strlen(pDevices) + 1;
		}
	}

	selectDevice(defaultDeviceId);
}

SoundSystem_OpenAL::~SoundSystem_OpenAL()
{
	releaseAllSounds();

	SoundDeviceContext* pContext = alcGetCurrentContext();
	if (pContext)
	{
		alcMakeContextCurrent(NULL);

		SoundDevice* pDevice = alcGetContextsDevice(pContext);
		alcDestroyContext(pContext);
		if (pDevice) alcCloseDevice(pDevice);
	}

	size_t const E = m_devices.size();
	for (size_t I = 0; I < E; ++I)
	{
		free(const_cast<char*>(m_devices[I].name));
		free(const_cast<char*>(m_devices[I].real_name));
	}
	m_devices.clear();
}

void SoundSystem_OpenAL::selectDevice(SoundDeviceID deviceId)
{
	SoundDeviceDescr& sdd = m_devices[deviceId];

	SoundDeviceContext* pContext = alcGetCurrentContext();
	SoundDevice* pDevice = NULL;
	if (pContext)
	{
		alcMakeContextCurrent(NULL);

		pDevice = alcGetContextsDevice(pContext);
		alcDestroyContext(pContext);
		if (pDevice) alcCloseDevice(pDevice);
	}

	pDevice = alcOpenDevice(sdd.name);

	pContext = alcCreateContext(pDevice, NULL);

	alcMakeContextCurrent(pContext);
}

SoundDeviceID SoundSystem_OpenAL::selectedDevice() const
{
	SoundDeviceID sdId = SoundDeviceID(~0);

	SoundDeviceContext* pContext	= alcGetCurrentContext();
	SoundDevice* pDevice			= alcGetContextsDevice(pContext);
	const char* sDeviceName			= alcGetString(pDevice, ALC_DEVICE_SPECIFIER);

	size_t const E = m_devices.size();
	for (size_t I = 0; I < E; ++I)
	{
		if (!strcmp(m_devices[I].real_name, sDeviceName))
		{
			sdId = SoundDeviceID(I);
			break;
		}
	}

	return (sdId);
}

ISound* SoundSystem_OpenAL::createSound(const char* sSoundFile)
{
	ISound* pSound = MEM_NEW(*m_pAllocator, Sound, m_pAllocator, sSoundFile);
	m_sounds.push_back(pSound);
	return (pSound);
}

void SoundSystem_OpenAL::destroySound(ISound*& pSound)
{
	if (pSound)
	{
		if (pSound->isPlaying())
		{
			pSound->stop();
		}
		
		SOUNDS::iterator it = std::find(m_sounds.begin(), m_sounds.end(), pSound);
		if (it != m_sounds.end())
			m_sounds.erase(it);
		MEM_DELETE(*m_pAllocator, ISound, pSound);
		pSound = NULL;
	}
	pSound = NULL;
}

void SoundSystem_OpenAL::onFrame(float delta)
{
	size_t const E = m_sounds.size();
	for (size_t I = 0; I < E; ++I)
		m_sounds[I]->onFrame(delta);
}

void SoundSystem_OpenAL::stopAllSounds()
{
	size_t const E = m_sounds.size();
	for (size_t I = 0; I < E; ++I)
		m_sounds[I]->stop();
}

void SoundSystem_OpenAL::pauseAllSounds()
{
	size_t const E = m_sounds.size();
	for (size_t I = 0; I < E; ++I)
		m_sounds[I]->pause();
}

void SoundSystem_OpenAL::releaseAllSounds()
{
	size_t const E = m_sounds.size();
	for (size_t I = 0; I < E; ++I)
		MEM_DELETE(*m_pAllocator, ISound, m_sounds[I]);
	m_sounds.clear();
}

float SoundSystem_OpenAL::getMasterVolume() const
{
	float fVolume; alGetListenerf(AL_GAIN, &fVolume);
	return (fVolume);
}

void SoundSystem_OpenAL::setMasterVolume(float fVolume)
{
	// 0 - 1
	if (fVolume > 1.f)			fVolume = 1.f;
	else if (fVolume < 0.001f)	fVolume = 0.001f;

	alListenerf(AL_GAIN, fVolume);
}
