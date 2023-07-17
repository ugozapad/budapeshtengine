#include "pch.h"
#include "engine/iosdriver.h"

class OsDriverWin32 : public IOsDriver
{
public:
	OsDriverWin32();
	~OsDriverWin32();

	void init() override;
	void shutdown() override;

	const char* getCurrentDirectory() override;

	bool isDirectoryExist(const char* path) override;

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

bool OsDriverWin32::isDirectoryExist(const char* path)
{
	DWORD dw_attribute = GetFileAttributesA(path);
	if (dw_attribute == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!
	if (dw_attribute & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!
	return false;
}

#endif // _MSC_VER
