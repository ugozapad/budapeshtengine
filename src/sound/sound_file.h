#ifndef SOUNDFILE_H
#define SOUNDFILE_H
#pragma once

enum SoundFileSeekMode_
{
	SoundFileSeekMode_Begin,
	SoundFileSeekMode_Current,
	SoundFileSeekMode_End
};
typedef unsigned int SoundFileSeekMode;

// NOTE : DMan to all : Do not change uint32_t to size_t because I can't guarantee that vorbis can work with x64 properly!
class ISoundReader
{
public:
	virtual ~ISoundReader() { }

	virtual uint32_t	read			(void* pBuffer, uint32_t dwCount) = 0;

	virtual int32_t		getFrequency	() const = 0;
	virtual uint32_t	getChannelsNum	() const = 0;

	virtual uint32_t	tell			() const = 0;
	virtual int32_t		seek			(int32_t offset, SoundFileSeekMode mode) = 0;

	virtual int32_t		getFormat			() const = 0;
	virtual uint32_t	getBufferSize		() const = 0;
};

// OGG

typedef struct OggVorbis_File	OggSoundFile;
typedef struct vorbis_info		OggSoundFileInfo;

class OggSoundFileReader : public ISoundReader
{
public:
	OggSoundFileReader(const char* sFileName);
	~OggSoundFileReader();

	uint32_t		read			(void* pBuffer, uint32_t szCount) override;

	int32_t			getFrequency	() const override;
	uint32_t		getChannelsNum	() const override;


	uint32_t		tell			() const override;
	int32_t			seek			(int32_t offset, SoundFileSeekMode mode) override;

	int32_t			getFormat		() const override { return (m_iFormat); }
	uint32_t		getBufferSize	() const override { return (m_dwBufferSize); }

private:
	OggSoundFileInfo*	m_pInfo;
	OggSoundFile*		m_pFile;

	int32_t				m_iFormat;
	uint32_t			m_dwBufferSize;
};

#endif
