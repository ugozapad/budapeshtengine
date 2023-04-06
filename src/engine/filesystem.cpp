#include "engine/allocator.h"
#include "engine/filesystem.h"

#include <stdio.h>

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

	IReader* openRead(const char* filename) override;
	IWriter* openWrite(const char* filename) override;

	void deleteReader(IReader*& reader) override;
	void deleteWriter(IWriter*& writer) override;
};

IFileSystem* IFileSystem::create() {
	return MEM_NEW(*g_default_allocator, FileSystem);
}

void IFileSystem::destroy(IFileSystem*& fs_ptr) {
	MEM_DELETE(*g_default_allocator, IFileSystem, fs_ptr);
	fs_ptr = nullptr;
}

IReader* FileSystem::openRead(const char* filename) {
	return MEM_NEW(*g_default_allocator, FileReader, filename);
}

IWriter* FileSystem::openWrite(const char* filename) {
	return MEM_NEW(*g_default_allocator, FileWriter, filename);
}

void FileSystem::deleteReader(IReader*& reader) {
	MEM_DELETE(*g_default_allocator, IReader, reader);
}

void FileSystem::deleteWriter(IWriter*& writer) {
	MEM_DELETE(*g_default_allocator, IWriter, writer);
}
