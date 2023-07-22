#include "pch.h"
#include "engine/objectfactory.h"

ENGINE_API ObjectFactory* g_object_factory = nullptr;

ObjectFactory::ObjectFactory()
{
}

ObjectFactory::~ObjectFactory()
{
}

TypedObject* ObjectFactory::createByName(const char* classname)
{
	for (auto it : m_objectCreationInfos)
	{
		if (strcmp(it.classname, classname) == 0)
			return it.create_proc();
	}

	return nullptr;
}
