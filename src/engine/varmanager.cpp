#include "pch.h"
#include "varmanager.h"
#include "filesystem.h"

#include <algorithm>

VarManager g_VarManager;

Var::Var(const char* name, const char* value, const char* description, int flags) :
	m_name(name), m_description(description), m_flags(flags)
{
	m_value.resize(strlen(value) + 1);
	strcpy(m_value.data(), value);
}

Var::~Var()
{
	m_description = nullptr;
	m_name = nullptr;
}

const char* Var::GetName()
{
	return m_name;
}

const char* Var::GetDescription()
{
	return m_description;
}

const char* Var::GetStringValue()
{
	return m_value.data();
}

float Var::GetFloatValue()
{
	return atof(m_value.data());
}

int Var::GetIntValue()
{
	return atoi(m_value.data());
}

void Var::SetStringValue(const char* str)
{
	//m_value.clear();
	m_value.resize(strlen(str) + 1);
	strcpy(m_value.data(), str);
}

void Var::SetFloatValue(float value)
{
	char buffer[64];
	snprintf(buffer, sizeof(buffer), "%f", value);

	SetStringValue(buffer);
}

void Var::SetIntValue(int value)
{
	char buffer[64];
	snprintf(buffer, sizeof(buffer), "%i", value);

	SetStringValue(buffer);
}

void VarManager::RegisterVar(Var* pVar)
{
	ASSERT(pVar);

	m_vars.push_back(pVar);
	SortVars();
}

Var* VarManager::GetVar(const char* name)
{
	for (auto it : m_vars)
	{
		if (it && strcmp(it->GetName(), name) == 0)
			return it;
	}

	return nullptr;
}

void VarManager::Save(const char* filename)
{
	IWriter* writer = g_file_system->openWrite(filename);
	for (size_t I = 0; I < m_vars.size(); ++I)
	{
		Var* var = m_vars[I];
		if (var && !(var->m_flags & VARFLAG_NOSAVE))
		{
			char buffer[256];
			int len = sprintf(buffer, "set %-30s   %s\n", var->GetName(), var->GetStringValue());

			writer->write(buffer, len);
		}
	}

	g_file_system->deleteWriter(writer);
}

void VarManager::SortVars()
{
	std::sort(m_vars.begin(), m_vars.end(), 
		[](Var* lhs, Var* rhs)
		{
			return strlen(lhs->GetName()) < strlen(rhs->GetName());
		}
	);
}
