#ifndef IOSDRIVER_H
#define IOSDRIVER_H

class IOsDriver
{
public:
	virtual ~IOsDriver() {}

	virtual void Init() = 0;
	virtual void Shutdown() = 0;

	virtual const char* GetCurrentAppDirectory() = 0;

	virtual bool IsDirectoryExist(const char* path) = 0;
};

extern IOsDriver* g_os_driver;

#endif // !IOSDRIVER_H
