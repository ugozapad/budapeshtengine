#ifndef VARMANAGER_H
#define VARMANAGER_H

#include <map>

typedef enum
{
	VARFLAG_NONE = 1 << 0,
	VARFLAG_NOSAVE = 1 << 1,
	VARFLAG_SERVER_PROTECT = 1 <<2,
	VARFLAG_DEVELOPER_MODE = 1 << 3,
	VARFLAG_CHEAT = 1 << 4
} varFlags_t;

class VarManager;

class ENGINE_API Var
{
	friend class VarManager;
public:
	Var(const char* name, const char* value, const char* description, int flags);
	~Var();

	const char* GetName();
	const char* GetDescription();
	const char* GetStringValue();
	float GetFloatValue();
	int GetIntValue();

	void SetStringValue(const char* str);
	void SetFloatValue(float value);
	void SetIntValue(int value);

private:
	Array<char> m_value;
	const char* m_name;
	const char* m_description;

	int m_flags;

};

class ENGINE_API VarManager
{
public:
	void RegisterVar(Var* pVar);

	Var* GetVar(const char* name);

	void Save(const char* filename);

private:
	void SortVars();

private:
	Array<Var*> m_vars;
};

extern VarManager g_VarManager;

#endif // !VARMANAGER_H
