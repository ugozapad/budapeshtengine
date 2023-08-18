#include "pch.h"
#include "sound.h"

#define SOUND_BUFFERS_COUNT 4

#define isValidBuffer alIsBuffer
#define isValidSource alIsSource

Sound::Sound(const char* sFileName)
	: m_pSoundFile(NULL), m_pBuffer(NULL),
	m_pSoundBuffers(NULL), m_dwSoundSource(0),
	m_state(SoundState_Stopped), m_nextState(SoundState_Stopped),
	m_flags(SoundFlags_OwnsFile)
{
	m_pSoundFile = mem_new<OggSoundFileReader>(sFileName);

	initSoundBuffers(SOUND_BUFFERS_COUNT);
}

Sound::Sound(ISoundReader* pSoundFile)
	: m_pSoundFile(pSoundFile), m_pBuffer(NULL),
	m_pSoundBuffers(NULL), m_dwSoundSource(0),
	m_state(SoundState_Stopped), m_nextState(SoundState_Stopped),
	m_flags(0)
{
	initSoundBuffers(SOUND_BUFFERS_COUNT);
}

Sound::~Sound()
{
	if (isValidSource(m_dwSoundSource))
	{
		setLooped(false);
		onStateSwitch(SoundState_Stopped);
	}

	if (m_flags & SoundFlags_OwnsFile)
	{
		SAFE_DELETE(m_pSoundFile);
	}

	alDeleteSources(1, &m_dwSoundSource);
	alDeleteBuffers(SOUND_BUFFERS_COUNT, m_pSoundBuffers);
	mem_free(m_pSoundBuffers);

	mem_free(m_pBuffer);
}

void Sound::play()
{
	if (!isPlaying())
		switchState(SoundState_Playing);
}

void Sound::stop()
{
	if (isPlaying() || isPaused())
		switchState(SoundState_Stopped);
}

void Sound::pause()
{
	if (isPlaying())
		switchState(SoundState_Paused);
}

bool Sound::isPlaying() const
{
	return (m_state == SoundState_Playing);
}

bool Sound::isPaused() const
{
	return (m_state == SoundState_Paused);
}

bool Sound::isLooped() const
{
	return (m_flags & SoundFlags_Looped);
}

void Sound::setLooped(bool bLooped)
{
	if (bLooped)
		m_flags |= SoundFlags_Looped;
	else
		m_flags &= ~SoundFlags_Looped;
}

float Sound::volume() const
{
	float fVolume; alGetSourcef(m_dwSoundSource, AL_GAIN, &fVolume);
	return (fVolume);
}

void Sound::setVolume(float fVolume)
{
	// 0 - 1
	if (fVolume > 1.f)			fVolume = 1.f;
	else if (fVolume < 0.001f)	fVolume = 0.001f;
	alSourcef(m_dwSoundSource, AL_GAIN, fVolume);
}

void Sound::onFrame(float delta)
{
	if (isValidSource(m_dwSoundSource))
	{
		if (m_nextState != m_state)
		{
			onStateSwitch(m_nextState);
			m_state = m_nextState;
		}
		else
		{
			switch (m_state)
			{
			case SoundState_Stopped:
			{
				if (isLooped() && getRealState() == AL_STOPPED)
				{
					play();
				}
			}break;
			case SoundState_Paused: break;
			case SoundState_Playing:
			{
				int32_t iBuffersProcessed = 0;
				alGetSourcei(m_dwSoundSource, AL_BUFFERS_PROCESSED, &iBuffersProcessed);
				if (iBuffersProcessed > 0)
				{
					uint32_t dwBuffer; alSourceUnqueueBuffers(m_dwSoundSource, 1, &dwBuffer);
					fillBuffer(dwBuffer);
				}

				if (getRealState() != AL_PLAYING)
				{
					int32_t iQueuedBuffers; alGetSourcei(m_dwSoundSource, AL_BUFFERS_QUEUED, &iQueuedBuffers);
					if (iQueuedBuffers > 0)
					{
						alSourcePlay(m_dwSoundSource);
					}
					else
					{
						stop();
					}
				}
			}break;
			default: __assume(0);
			}
		}
	}
}

void Sound::onStateSwitch(SoundState next_state)
{
	switch (next_state)
	{
	case SoundState_Playing:
	{
		if (!isPlaying() && !isPaused())
			refillBuffers();
		alSourcePlay(m_dwSoundSource);
	}break;
	case SoundState_Stopped:
	{
		alSourceStop		(m_dwSoundSource);
		alSourceRewind		(m_dwSoundSource);
		alSourcei			(m_dwSoundSource, AL_BUFFER, 0);
		m_pSoundFile->seek	(0, SoundFileSeekMode_Begin);
	}break;
	case SoundState_Paused: alSourcePause(m_dwSoundSource); break;
	default: __assume(0);
	}
}

void Sound::initSoundBuffers(uint32_t dwSoundBuffersCount)
{
	m_pSoundBuffers = mem_tcalloc<uint32_t>(dwSoundBuffersCount);

	alGenBuffers(dwSoundBuffersCount, m_pSoundBuffers);
	alGenSources(1, &m_dwSoundSource);

	m_pBuffer = mem_tcalloc<char>(m_pSoundFile->getBufferSize());
}

void Sound::fillBuffer(uint32_t dwBufferId)
{
	ASSERT(isValidBuffer(dwBufferId));

	uint32_t dwBytesReaded = m_pSoundFile->read(m_pBuffer, m_pSoundFile->getBufferSize());
	if (dwBytesReaded)
	{
		alBufferData(
			dwBufferId,
			m_pSoundFile->getFormat(),
			m_pBuffer,
			dwBytesReaded,
			m_pSoundFile->getFrequency()
		);
		queueBuffer(dwBufferId);
	}
}

void Sound::queueBuffer(uint32_t dwBufferId)
{
	alSourceQueueBuffers(
		m_dwSoundSource,
		1,
		&dwBufferId
	);
}

void Sound::refillBuffers()
{
	ASSERT(!isPlaying() && !isPaused());

	for (size_t i = 0; i < SOUND_BUFFERS_COUNT; ++i)
		fillBuffer(m_pSoundBuffers[i]);
}

int32_t Sound::getRealState() const
{
	int32_t iState; alGetSourcei(m_dwSoundSource, AL_SOURCE_STATE, &iState);
	return (iState);
}
