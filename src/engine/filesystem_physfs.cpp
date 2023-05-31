#ifdef ENABLE_PHYSFS

#include <SDL.h>
#include <physfs.h>

#include <stdio.h>

#include "engine/debug.h"
#include "engine/allocator.h"
#include "engine/iosdriver.h"
#include "engine/filesystem.h"

// File reader
class FileReaderPhysFS : public IReader {
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
	m_file_handle(nullptr) {
	m_file_handle = PHYSFS_openRead(filename);
	ASSERT_MSG(m_file_handle, "Unable to open file %s", filename);
}

FileReaderPhysFS::~FileReaderPhysFS() {
	if (m_file_handle) {
		PHYSFS_close(m_file_handle);
		m_file_handle = nullptr;
	}
}

void FileReaderPhysFS::seek(SeekWay seekway, long offset) {
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

size_t FileReaderPhysFS::tell() {
	return (size_t)PHYSFS_tell(m_file_handle);
}

size_t FileReaderPhysFS::read(void* data, size_t size) {
	return PHYSFS_read(m_file_handle, data, sizeof(char), size);
}

// File Writer
class FileWriterPhysFS : public IWriter {
public:
	FileWriterPhysFS(const char* filename);
	~FileWriterPhysFS();

	void seek(SeekWay seekway, long offset) override;
	size_t tell() override;
	size_t write(const void* data, size_t size) override;
private:
	PHYSFS_file* m_file_handle;
};

FileWriterPhysFS::FileWriterPhysFS(const char* filename) :
	m_file_handle(nullptr) {
	m_file_handle = PHYSFS_openWrite(filename);
	ASSERT_MSG(m_file_handle, "Unable to open file %s", filename);
}

FileWriterPhysFS::~FileWriterPhysFS() {
	if (m_file_handle) {
		PHYSFS_close(m_file_handle);
		m_file_handle = nullptr;
	}
}

void FileWriterPhysFS::seek(SeekWay seekway, long offset) {
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

size_t FileWriterPhysFS::tell() {
	return (size_t)PHYSFS_tell(m_file_handle);
}

size_t FileWriterPhysFS::write(const void* data, size_t size) {
	return PHYSFS_write(m_file_handle, data, sizeof(char), size);
}

// File system
class FileSystemPhysFS : public IFileSystem {
public:
	FileSystemPhysFS(IAllocator& allocator);
	~FileSystemPhysFS();

	bool fileExist(const char* filename) override;

	IReader* openRead(const char* filename) override;
	IWriter* openWrite(const char* filename) override;

	void deleteReader(IReader*& reader) override;
	void deleteWriter(IWriter*& writer) override;

private:
	IAllocator* m_allocator;
};

IFileSystem* IFileSystem::createPhysFS() {
	return MEM_NEW(*g_default_allocator, FileSystemPhysFS, *g_default_allocator);
}

FileSystemPhysFS::FileSystemPhysFS(IAllocator& allocator) :
	m_allocator(&allocator) {
	PHYSFS_init(NULL);

	const char* current_directory = IOsDriver::getInstance()->getCurrentDirectory();
	printf("Current path: %s\n", current_directory);
	PHYSFS_mount(current_directory, nullptr, 0);

	// mount arhives
	PHYSFS_mount("data_textures.zip", nullptr, 1);
	PHYSFS_mount("data_levels.zip", nullptr, 1);
	PHYSFS_mount("data_scripts.zip", nullptr, 1);
	PHYSFS_mount("data_shaders.zip", nullptr, 1);
	PHYSFS_mount("data_models.zip", nullptr, 1);
	PHYSFS_mount("data_sounds.zip", nullptr, 1);

#if 0
	char** fileList = PHYSFS_enumerateFiles(PHYSFS_getBaseDir());
	for (char** it = fileList; it != nullptr; it++)
	{
		if (*it && SDL_strstr(*it, ".zip"))
			PHYSFS_mount(*it, nullptr, 1);
	}
#endif
}

FileSystemPhysFS::~FileSystemPhysFS() {
	PHYSFS_deinit();
}

bool FileSystemPhysFS::fileExist(const char* filename) {
	return PHYSFS_exists(filename);
}

IReader* FileSystemPhysFS::openRead(const char* filename) {
	return MEM_NEW(*m_allocator, FileReaderPhysFS, filename);
}

IWriter* FileSystemPhysFS::openWrite(const char* filename) {
	return MEM_NEW(*m_allocator, FileWriterPhysFS, filename);
}

void FileSystemPhysFS::deleteReader(IReader*& reader) {
	MEM_DELETE(*m_allocator, IReader, reader);
}

void FileSystemPhysFS::deleteWriter(IWriter*& writer) {
	MEM_DELETE(*m_allocator, IWriter, writer);
}

#endif // ENABLE_PHYSFS
