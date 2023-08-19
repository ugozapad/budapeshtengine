#include "pch.h"
#include "engine/debug.h"
#include "engine/allocator.h"
#include "engine/iosdriver.h"
#include "engine/filesystem.h"

IFileSystem* g_file_system = nullptr;
static int s_stdio_seek[] = { SEEK_SET, SEEK_CUR, SEEK_END };

// File reader
class FileReader : public IReader {
public:
	FileReader(const char* filename);
	~FileReader();

	void seek(SeekWay seekway, long offset) override;
	size_t tell() override;
	size_t read(void* data, size_t size) override;
private:
	FILE* m_file_handle;
};

FileReader::FileReader(const char* filename) :
	m_file_handle(nullptr) {
	m_file_handle = fopen(filename, "rb");
	ASSERT_MSG(m_file_handle, "Unable to open file %s", filename);
}

FileReader::~FileReader() {
	if (m_file_handle) {
		fclose(m_file_handle);
		m_file_handle = nullptr;
	}
}

void FileReader::seek(SeekWay seekway, long offset) {
	fseek(m_file_handle, offset, s_stdio_seek[(int)seekway]);
}

size_t FileReader::tell() {
	return (size_t)ftell(m_file_handle);
}

size_t FileReader::read(void* data, size_t size) {
	return fread(data, sizeof(char), size, m_file_handle);
}

// File writer
class FileWriter : public IWriter {
public:
	FileWriter(const char* filename);
	~FileWriter();

	void seek(SeekWay seekway, long offset) override;
	size_t tell() override;
	size_t write(const void* data, size_t size) override;
private:
	FILE* m_file_handle;
};

FileWriter::FileWriter(const char* filename) :
	m_file_handle(nullptr) {
	m_file_handle = fopen(filename, "wb");
	ASSERT_MSG(m_file_handle, "TODO: Return null ptr instead FileReader instance when file doesn't exist on disk.");
}

FileWriter::~FileWriter() {
	if (m_file_handle) {
		fclose(m_file_handle);
		m_file_handle = nullptr;
	}
}

void FileWriter::seek(SeekWay seekway, long offset) {
	fseek(m_file_handle, offset, s_stdio_seek[(int)seekway]);
}

size_t FileWriter::tell() {
	return (size_t)ftell(m_file_handle);
}

size_t FileWriter::write(const void* data, size_t size) {
	return fwrite(data, sizeof(char), size, m_file_handle);
}

// Real file system
class FileSystem : public IFileSystem {
public:
	FileSystem() {}
	~FileSystem() {}

	bool fileExist(const char* filename) override;

	IReader* openRead(const char* filename) override;
	IWriter* openWrite(const char* filename) override;

	void deleteReader(IReader*& reader) override;
	void deleteWriter(IWriter*& writer) override;
};

IFileSystem* IFileSystem::Create()
{
	return mem_new<FileSystem>();
}

void IFileSystem::Destroy(IFileSystem*& fs_ptr)
{
	mem_delete(fs_ptr);
	fs_ptr = nullptr;
}

bool FileSystem::fileExist(const char* filename)
{
	ASSERT(filename);

	size_t length = strlen(filename);
	ASSERT(length != 0);

	if (filename && length > 0 && filename[length - 1] == '/')
		return g_os_driver->IsDirectoryExist(filename);

	FILE* f = fopen(filename, "rb");
	if (f) {
		fclose(f);
		return true;
	}

	return false;
}

IReader* FileSystem::openRead(const char* filename)
{
	return mem_new<FileReader>(filename);
}

IWriter* FileSystem::openWrite(const char* filename)
{
	return mem_new<FileWriter>(filename);
}

void FileSystem::deleteReader(IReader*& reader)
{
	mem_delete(reader);
	reader = nullptr;
}

void FileSystem::deleteWriter(IWriter*& writer)
{
	mem_delete(writer);
	writer = nullptr;
}
