#ifndef SOUND_H
#define SOUND_H
#pragma once

class IAllocator;
class Sound : public ISound
{
	enum SoundFlags_
	{
		SoundFlags_OwnsFile = (1 << 0),
		SoundFlags_Looped	= (1 << 1)
	};
	typedef uint32_t SoundFlags;
	enum SoundState_
	{
		SoundState_Stopped,
		SoundState_Playing,
		SoundState_Paused
	};
	typedef uint32_t SoundState;
public:
	Sound	(IAllocator* pAllocator, const char* sFileName);
	Sound	(IAllocator* pAllocator, ISoundReader* pSoundFile);
	~Sound	();

	void play	() override;
	void stop	() override;
	void pause	() override;


	bool isPlaying	() const override;
	bool isPaused	() const override;
	bool isLooped	() const override;


	void	setLooped	(bool bLooped) override;

	float	volume		() const override;
	void	setVolume	(float fVolume) override;

	void	onFrame		(float delta) override;

private:
	inline void switchState		(SoundState s) { m_nextState = s; }
	void		onStateSwitch	(SoundState s);

	void		initSoundBuffers(uint32_t dwSoundBuffersCount);

	void		fillBuffer		(uint32_t dwBufferId);
	void		queueBuffer		(uint32_t dwBufferId);
	void		refillBuffers	();

	int32_t		getRealState	() const;

private:
	SoundFlags m_flags;

	SoundState m_state;
	SoundState m_nextState;

	ISoundReader*	m_pSoundFile;

	char*			m_pBuffer;
	uint32_t		m_dwBufferSize;

	uint32_t*		m_pSoundBuffers;
	uint32_t		m_dwSoundSource;

	IAllocator*		m_pAllocator;
};

#endif
