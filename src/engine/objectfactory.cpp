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

void ObjectFactory::getObjectCreationInfos(Array<objectCreationInfo_t>& creationInfos)
{
	creationInfos.resize(m_objectCreationInfos.size());

	std::transform(m_objectCreationInfos.begin(), m_objectCreationInfos.end(), creationInfos.begin(),
		[](auto i) {
		return i;
	});

	//creationInfos = m_objectCreationInfos;
}
