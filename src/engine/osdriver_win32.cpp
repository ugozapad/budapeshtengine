#ifdef _MSC_VER

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "engine/iosdriver.h"

class OsDriverWin32 : public IOsDriver
{
public:
	OsDriverWin32();
	~OsDriverWin32();

	void init() override;
	void shutdown() override;

	const char* getCurrentDirectory() override;

private:
	char m_current_directory[256];
};

static OsDriverWin32 g_win32_os_driver;
IOsDriver* g_osdriver = (IOsDriver*)&g_win32_os_driver;

IOsDriver* IOsDriver::getInstance()
{
	return g_osdriver;
}

OsDriverWin32::OsDriverWin32()
{
	memset(m_current_directory, 0, sizeof(m_current_directory));
}

OsDriverWin32::~OsDriverWin32()
{
}

void OsDriverWin32::init()
{
}

void OsDriverWin32::shutdown()
{
}

const char* OsDriverWin32::getCurrentDirectory()
{
	if (GetCurrentDirectoryA(sizeof(m_current_directory), m_current_directory))
		return m_current_directory;

	return nullptr;
}

#endif // _MSC_VER
