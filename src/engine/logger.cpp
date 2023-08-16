#include "pch.h"

#include "engine/logger.h"
#include "engine/filesystem.h"

IWriter* g_log_writer = nullptr;

static const char* g_month[12] = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };
static int g_day_in_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
int build_id;

void CalculateBuildNumber()
{
	static int start_day = 3;
	static int start_month = 10;
	static int start_year = 2021;

	// Calculating build
	int build = 0, mnum = 0, dnum, ynum, mcnt;
	char mon[4];
	char buf[128];
	strcpy(buf, __DATE__);
	sscanf(buf, "%s %d %d", mon, &dnum, &ynum);
	for (int i = 0; i < 12; i++) {
#ifdef WIN32
		if (_stricmp(g_month[i], mon) == 0) {
#else
		if (strcasecmp(g_month[i], mon) == 0) {
#endif // WIN32
			mnum = i;
			break;
		}
	}

	build_id = (ynum - start_year) * 365 + dnum - start_day;

	for (int i = 0; i < mnum; ++i)
		build_id += g_day_in_month[i];

	for (int i = 0; i < start_month - 1; ++i)
		build_id -= g_day_in_month[i];
}

void LogOpen(const char* filename)
{
	char username[256];
	DWORD username_size = sizeof(username);

	if (!GetUserNameA(username, &username_size))
		strncat(username, "unnamed", username_size);

	char logFilename[256];
	snprintf(logFilename, sizeof(logFilename), "%s_%s.log", filename, username);

	g_log_writer = g_file_system->openWrite(logFilename);
	assert(g_log_writer);

	struct tm newtime;
	__time32_t aclock;

	char buffer[32];
	_time32(&aclock);
	_localtime32_s(&newtime, &aclock);
	errno_t errNum = asctime_s(buffer, 32, &newtime);
	Msg("Log started: %s", buffer);

	CalculateBuildNumber();
	Msg("'%s' build %d, %s", filename, build_id, __DATE__);
}

void LogClose()
{
	if (g_log_writer)
	{
		// flush log
		//g_logWriter->flush();

		delete g_log_writer;
		g_log_writer = nullptr;
	}
}

void Msg(const char* msg, ...)
{
	static char buffer[1024];
	va_list args;
	va_start(args, msg);
	vsnprintf(buffer, sizeof(buffer), msg, args);
	va_end(args);

	strcat(buffer, "\n");

	OutputDebugStringA(buffer);

	// write to console
	fwrite(buffer, sizeof(char), strlen(buffer), stdout);

	if (g_log_writer)
		g_log_writer->write(buffer, strlen(buffer));
}
