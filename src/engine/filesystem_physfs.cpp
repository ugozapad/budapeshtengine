#ifdef ENABLE_PHYSFS

#include <SDL.h>
#include <physfs.h>

#include <stdio.h>

#include "engine/debug.h"
#include "engine/allocator.h"
#include "engine/iosdriver.h"
#include "engine/filesystem.h"
#include "engine/logger.h"

// File reader
class FileReaderPhysFS : public IReader
{
public:
	FileReaderPhysFS(const char* filename);
	~FileReaderPhysFS();

	void seek(SeekWay seekway, long offset) override;
	size_t tell() override;
	size_t read(void* data, size_t size) override;
private:
	PHYSFS_file* m_file_handle;
};

FileReaderPhysFS::FileReaderPhysFS(const char* filename) :
	m_file_handle(nullptr)
{
	m_file_handle = PHYSFS_openRead(filename);
	ASSERT_MSG(m_file_handle, "Unable to open file %s", filename);
}

FileReaderPhysFS::~FileReaderPhysFS()
{
	if (m_file_handle)
	{
		PHYSFS_close(m_file_handle);
		m_file_handle = nullptr;
	}
}

void FileReaderPhysFS::seek(SeekWay seekway, long offset)
{
	switch (seekway)
	{
	case Begin:
		PHYSFS_seek(m_file_handle, offset);
		break;
	case Current:
	{
		PHYSFS_sint64 current_offset = PHYSFS_tell(m_file_handle);
		PHYSFS_seek(m_file_handle, current_offset + offset);
		break;
	}
	case End:
	{
		PHYSFS_sint64 file_length = PHYSFS_fileLength(m_file_handle);
		PHYSFS_seek(m_file_handle, file_length - offset);
		break;
	}
	}
}

size_t FileReaderPhysFS::tell()
{
	return (size_t)PHYSFS_tell(m_file_handle);
}

size_t FileReaderPhysFS::read(void* data, size_t size)
{
	return PHYSFS_read(m_file_handle, data, sizeof(char), size);
}

// File Writer
class FileWriterPhysFS : public IWriter
{
public:
	FileWriterPhysFS(const char* filename);
	~FileWriterPhysFS();

	void seek(SeekWay seekway, long offset) override;
	size_t tell() override;
	size_t write(const void* data, size_t size) override;
private:
	PHYSFS_file* m_file_handle;
};

FileWriterPhysFS::FileWriterPhysFS(const char* filename):
	m_file_handle(nullptr)
{
	if (!g_file_system->fileExist(filename))
	{
		FILE* f = fopen(filename, "wb");
		if (f) fclose(f);
	}

	m_file_handle = PHYSFS_openWrite(filename);
	ASSERT_MSG(m_file_handle, "Unable to open file %s", filename);
}

FileWriterPhysFS::~FileWriterPhysFS()
{
	if (m_file_handle)
	{
		PHYSFS_close(m_file_handle);
		m_file_handle = nullptr;
	}
}

void FileWriterPhysFS::seek(SeekWay seekway, long offset)
{
	switch (seekway)
	{
	case Begin:
		PHYSFS_seek(m_file_handle, offset);
		break;
	case Current:
	{
		PHYSFS_sint64 current_offset = PHYSFS_tell(m_file_handle);
		PHYSFS_seek(m_file_handle, current_offset + offset);
		break;
	}
	case End:
	{
		PHYSFS_sint64 file_length = PHYSFS_fileLength(m_file_handle);
		PHYSFS_seek(m_file_handle, file_length - offset);
		break;
	}
	}
}

size_t FileWriterPhysFS::tell()
{
	return (size_t)PHYSFS_tell(m_file_handle);
}

size_t FileWriterPhysFS::write(const void* data, size_t size)
{
	return PHYSFS_write(m_file_handle, data, sizeof(char), size);
}

// File system
class FileSystemPhysFS : public IFileSystem
{
public:
	FileSystemPhysFS();
	~FileSystemPhysFS();

	bool fileExist(const char* filename) override;

	IReader* openRead(const char* filename) override;
	IWriter* openWrite(const char* filename) override;

	void deleteReader(IReader*& reader) override;
	void deleteWriter(IWriter*& writer) override;
};

IFileSystem* IFileSystem::createPhysFS()
{
	return new FileSystemPhysFS();
}

FileSystemPhysFS::FileSystemPhysFS()
{
	PHYSFS_init(NULL);

	const char* current_directory = IOsDriver::getInstance()->getCurrentDirectory();
	Msg("Current path: %s", current_directory);
	PHYSFS_mount(current_directory, nullptr, 0);

	// mount arhives
	char** file_list = PHYSFS_enumerateFiles("/");
	for (char** it = file_list; *it != NULL; it++)
	{
		if (SDL_strstr(*it, ".zip")) {
			Msg("found archive: %s", *it);
			PHYSFS_mount(*it, nullptr, 1);
		}
	}
}

FileSystemPhysFS::~FileSystemPhysFS()
{
	PHYSFS_deinit();
}

bool FileSystemPhysFS::fileExist(const char* filename)
{
	return PHYSFS_exists(filename);
}

IReader* FileSystemPhysFS::openRead(const char* filename)
{
	return new FileReaderPhysFS(filename);
}

IWriter* FileSystemPhysFS::openWrite(const char* filename)
{
	PHYSFS_setWriteDir("");

	return new FileWriterPhysFS(filename);
}

void FileSystemPhysFS::deleteReader(IReader*& reader)
{
	delete reader;
	reader = nullptr;
}

void FileSystemPhysFS::deleteWriter(IWriter*& writer)
{
	delete writer;
	writer = nullptr;
}

#endif // ENABLE_PHYSFS
