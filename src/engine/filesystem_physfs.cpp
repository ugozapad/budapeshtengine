#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "engine/allocator.h"
#include "engine/filesystem.h"

#include <physfs.h>

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
	FileSystemPhysFS();
	~FileSystemPhysFS();

	IReader* openRead(const char* filename) override;
	IWriter* openWrite(const char* filename) override;

	void deleteReader(IReader*& reader) override;
	void deleteWriter(IWriter*& writer) override;
};

IFileSystem* IFileSystem::createPhysFS() {
	return MEM_NEW(*g_default_allocator, FileSystemPhysFS);
}

FileSystemPhysFS::FileSystemPhysFS() {
	PHYSFS_init(NULL);

#ifdef WIN32
	char buffer[1024];
	GetCurrentDirectoryA(sizeof(buffer), buffer);
	PHYSFS_addToSearchPath(buffer, 0);
#endif // WIN32

	PHYSFS_addToSearchPath("data.zip", 1);
}

FileSystemPhysFS::~FileSystemPhysFS() {
	PHYSFS_deinit();
}

IReader* FileSystemPhysFS::openRead(const char* filename) {
	return MEM_NEW(*g_default_allocator, FileReaderPhysFS, filename);
}

IWriter* FileSystemPhysFS::openWrite(const char* filename) {
	return MEM_NEW(*g_default_allocator, FileWriterPhysFS, filename);
}

void FileSystemPhysFS::deleteReader(IReader*& reader) {
	MEM_DELETE(*g_default_allocator, IReader, reader);
}

void FileSystemPhysFS::deleteWriter(IWriter*& writer) {
	MEM_DELETE(*g_default_allocator, IWriter, writer);
}
