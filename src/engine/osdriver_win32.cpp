#include "pch.h"
#include "engine/iosdriver.h"

#ifdef _MSC_VER

class OsDriverWin32 : public IOsDriver
{
public:
	OsDriverWin32();
	~OsDriverWin32();

	void Init() override;
	void Shutdown() override;

	const char* GetCurrentAppDirectory() override;

	bool IsDirectoryExist(const char* path) override;

private:
	char m_current_directory[256];
};

static OsDriverWin32 g_win32_os_driver;
IOsDriver* g_os_driver = (IOsDriver*)&g_win32_os_driver;

OsDriverWin32::OsDriverWin32()
{
	memset(m_current_directory, 0, sizeof(m_current_directory));
}

OsDriverWin32::~OsDriverWin32()
{
}

void OsDriverWin32::Init()
{
	
}

void OsDriverWin32::Shutdown()
{
}

const char* OsDriverWin32::GetCurrentAppDirectory()
{
	if (GetCurrentDirectoryA(sizeof(m_current_directory), m_current_directory))
		return m_current_directory;

	return nullptr;
}

bool OsDriverWin32::IsDirectoryExist(const char* path)
{
	DWORD dw_attribute = GetFileAttributesA(path);
	if (dw_attribute == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!
	if (dw_attribute & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!
	return false;
}

#endif // _MSC_VER
