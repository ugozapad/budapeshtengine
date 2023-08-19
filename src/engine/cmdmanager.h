#ifndef CMDMANAGER_H
#define CMDMANAGER_H

#include <string>
#include <map>

class IWriter;

class ENGINE_API CmdArgs
{
public:
	CmdArgs(const std::string& str);

	void parse(const std::string& str);

	const char* GetArgv(int argc) const;
	int GetArgc() const;

private:
	Array<std::string> m_args;
};

typedef void (*cmdFunction_t) (const CmdArgs& args);

class ENGINE_API CmdManager
{
public:
	void Init();
	void Shutdown();

	void Execute(const char* buffer);
	void ExecuteBuffered(const char* buffer);
	void ExecuteNow(const char* buffer);

	void Register(const char* name, cmdFunction_t function);

	bool HasCmd(const char* name);

	static void PrintCmds(const CmdArgs& args);
	static void Exec(const CmdArgs& args);

	void SaveCmdsToFile(IWriter* file);

private:
	void Execute(const CmdArgs& args);

private:
	std::map<std::string, cmdFunction_t> m_cmds;
	std::string m_buffer;
};

extern ENGINE_API CmdManager g_cmd_manager;

#endif