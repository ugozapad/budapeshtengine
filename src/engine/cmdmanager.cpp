#include "pch.h"
#include "cmdmanager.h"
#include "filesystem.h"

ENGINE_API CmdManager g_cmd_manager;

void CmdManager::Init()
{
	Register("cmdlist", CmdManager::PrintCmds);
	Register("exec", CmdManager::Exec);
}

void CmdManager::Shutdown()
{
}

void CmdManager::Execute(const char* buffer)
{
	return ExecuteNow(buffer);
}

void CmdManager::ExecuteBuffered(const char* buffer)
{
}

void CmdManager::ExecuteNow(const char* buffer)
{
	return Execute(CmdArgs(buffer));
}

void CmdManager::Register(const char* name, cmdFunction_t function)
{
	if (HasCmd(name)) {
		Msg("CmdManager::registerCmd: can't register command '%s' twice!", name);
		return;
	}

	m_cmds[name] = function;
}

bool CmdManager::HasCmd(const char* name)
{
	auto var = m_cmds.find(name);
	if (var == m_cmds.end()) {
		return false;
	}

	return true;
}

void CmdManager::PrintCmds(const CmdArgs& args)
{
	for (auto it : g_cmd_manager.m_cmds) {
		Msg("%s", it.first.c_str());
	}
}

void CmdManager::Exec(const CmdArgs& args)
{
}

void CmdManager::SaveCmdsToFile(IWriter* file)
{
	if (!file)
		return;
	
	for (auto it : m_cmds)
	{
		char buffer[256];
		int len = sprintf(buffer, "%-30s\n", it.first.c_str());

		file->write(buffer, len);
	}
}

void CmdManager::Execute(const CmdArgs& args)
{
	for (auto it : m_cmds) {
		if (strcmp(args.GetArgv(0), it.first.c_str()) == 0) {
			if (!it.second) {
				break;
			} else {
				it.second(args);
				return;
			}
		}
	}
}

CmdArgs::CmdArgs(const std::string& str)
{
	parse(str);
}

void CmdArgs::parse(const std::string& str)
{
	char* p = (char*)str.c_str();
	char* tok = strtok(p, " \r\t\n");

	while (tok) {
		if (!tok || tok[0] == 0) {
			break;
		}

		m_args.push_back(tok);

		tok = strtok(p, " \r\t\n");
	}
}

const char* CmdArgs::GetArgv(int argc) const
{
	if (!m_args.empty() && argc <= m_args.size()) {
		return m_args[argc].c_str();
	}

	return nullptr;
}

int CmdArgs::GetArgc() const
{
	return int(m_args.size());
}
