#ifndef FILESYSTEM_H
#define FILESYSTEM_H

enum SeekWay
{
	Begin,
	Current,
	End
};

class IReader {
public:
	virtual ~IReader() {}
	
	virtual void seek(SeekWay seekway, long offset) = 0;
	virtual size_t tell() = 0;
	virtual size_t read(void* data, size_t size) = 0;
};

class IWriter {
public:
	virtual ~IWriter() {}

	virtual void seek(SeekWay seekway, long offset) = 0;
	virtual size_t tell() = 0;
	virtual size_t write(const void* data, size_t size) = 0;
};

class IFileSystem {
public:
	static IFileSystem* create();
	static void destroy(IFileSystem*& fs_ptr);

public:
	virtual ~IFileSystem() {}

	virtual IReader* openRead(const char* filename) = 0;
	virtual IWriter* openWrite(const char* filename) = 0;

	virtual void deleteReader(IReader*& reader) = 0;
	virtual void deleteWriter(IWriter*& writer) = 0;
};

extern IFileSystem* g_file_system;

#endif // !FILESYSTEM_H
