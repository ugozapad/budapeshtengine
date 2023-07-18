#include "pch.h"
#include "sound_file.h"

#include <algorithm>

#pragma comment(lib, "ogg.lib")
#pragma comment(lib, "vorbis.lib")
#pragma comment(lib, "vorbisfile.lib")

#include <ogg/os_types.h>
#include <vorbis/vorbisfile.h>

static void correct_sound_info(
	uint32_t dwChannels, int32_t iFrequency, // in (channels num & frequency)
	int32_t& iFormat, uint32_t dwBufferSize // out (format & buffer size)
)
{
	switch (dwChannels)
	{
	case 1:
	{
		iFormat = AL_FORMAT_MONO16;
		dwBufferSize = uint32_t(iFrequency >> 1);
		dwBufferSize -= (dwBufferSize % 2);
	}break;
	case 2:
	{
		iFormat = AL_FORMAT_STEREO16;
		dwBufferSize = uint32_t(iFrequency);
		dwBufferSize -= (dwBufferSize % 4);
	}break;
	case 4:
	{
		iFormat = alGetEnumValue("AL_FORMAT_QUAD16");
		dwBufferSize = uint32_t(iFrequency * 2);
		dwBufferSize -= (dwBufferSize % 8);
	}break;
	case 6:
	{
		iFormat = alGetEnumValue("AL_FORMAT_51CHN16");
		dwBufferSize = uint32_t(iFrequency * 3);
		dwBufferSize -= (dwBufferSize % 12);
	}break;
	default: FATAL("Unsupported channels num: %d", iFrequency);
	}
#ifndef NDEBUG
	ASSERT(iFormat > 0);
#endif
}

static void prepare_buffer(void* pBuffer, uint32_t dwBufferSize, uint32_t dwChannels)
{
	// Mono, Stereo and 4-Channel files decode into the same channel order as WAVEFORMATEXTENSIBLE,
	// however 6-Channels files need to be re-ordered
	if (dwChannels == 6)
	{
		uint16_t* pSamples = static_cast<uint16_t*>(pBuffer);
		for (uint64_t ullSamples = 0; ullSamples < uint64_t(dwBufferSize >> 1); ullSamples += 6)
		{
			// WAVEFORMATEXTENSIBLE Order : FL, FR, FC, LFE, RL, RR
			// OggVorbis Order            : FL, FC, FR,  RL, RR, LFE
			std::swap(pSamples[ullSamples + 1], pSamples[ullSamples + 2]);
			std::swap(pSamples[ullSamples + 3], pSamples[ullSamples + 5]);
			std::swap(pSamples[ullSamples + 4], pSamples[ullSamples + 5]);
		}
	}
}

OggSoundFileReader::OggSoundFileReader(const char* sFileName)
	: m_pInfo(NULL), m_pFile(NULL)
{
	m_pFile = static_cast<OggSoundFile*>(
		g_allocator->allocate(
			sizeof(OggSoundFile),
			alignof(OggSoundFile)
		)
	);

	int32_t iOpenResult = ov_fopen(sFileName, m_pFile);
	ASSERT_MSG(iOpenResult == 0, "Unable to open sound file: %s", sFileName);

	m_pInfo = ov_info(m_pFile, -1);
	ASSERT_MSG(m_pInfo, "Unable to read OGG-info from file: %s", sFileName);

	correct_sound_info(
		m_pInfo->channels,
		m_pInfo->rate,
		m_iFormat,
		m_dwBufferSize
	);
}

OggSoundFileReader::~OggSoundFileReader()
{
	if (m_pFile)
	{
		m_pInfo = NULL;
		ov_clear(m_pFile);
		g_allocator->deallocate(m_pFile);
		m_pFile = NULL;
	}
}

uint32_t OggSoundFileReader::read(void* pBuffer, uint32_t dwCount)
{
	uint32_t dwReadedBytes	= 0;
	char* pBufferCharPtr	= static_cast<char*>(pBuffer);
	int32_t iCurrentSection = 0;
	while (true)
	{
		uint32_t dwDecodeSize = ov_read(
			m_pFile,
			pBufferCharPtr + dwReadedBytes,
			dwCount - dwReadedBytes,
			0,
			2,
			1,
			&iCurrentSection
		);
		if (dwDecodeSize > 0)
		{
			dwReadedBytes += dwDecodeSize;

			if (dwReadedBytes >= dwCount)
				break;
		}
		else break;
	}

	prepare_buffer(pBuffer, dwReadedBytes, getChannelsNum());

	return (dwReadedBytes);
}

int32_t OggSoundFileReader::getFrequency() const
{
	return (m_pInfo->rate);
}

uint32_t OggSoundFileReader::getChannelsNum() const
{
	return (m_pInfo->channels);
}

uint32_t OggSoundFileReader::tell() const
{
	return (ftell(static_cast<FILE*>(m_pFile->datasource)));
}

int32_t OggSoundFileReader::seek(int32_t offset, SoundFileSeekMode mode)
{
	int32_t iOrigin = -1;
	switch (mode)
	{
	case SoundFileSeekMode_Begin:	iOrigin = SEEK_SET; break;
	case SoundFileSeekMode_Current: iOrigin = SEEK_CUR; break;
	case SoundFileSeekMode_End:		iOrigin = SEEK_END; break;
	default: __assume(0);
	}
#ifndef NDEBUG
	if (!ov_seekable(m_pFile))
		FATAL("Not seekable file!");
#endif
	return (
		fseek(
			static_cast<FILE*>(m_pFile->datasource),
			offset,
			iOrigin
		)
	);
}
