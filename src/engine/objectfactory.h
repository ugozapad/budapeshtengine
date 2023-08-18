#ifndef OBJECTFACTORY_H
#define OBJECTFACTORY_H

#include "engine/array.h"
#include "engine/object.h"

class TypedObject;
typedef TypedObject* (*pfnCreateObject_t)();

template <typename T>
T* CreateObjectTemplated() {
	return mem_new<T>();
}

struct objectCreationInfo_t {
	typeId_t id;
	const char* classname;
	pfnCreateObject_t create_proc;
};

class ENGINE_API ObjectFactory {
public:
	ObjectFactory();
	~ObjectFactory();

	template <typename T>
	void RegisterObject(const char* classname);

	template <typename T>
	T* CreateObject();

	TypedObject* CreateObjectByName(const char* classname);

	void GetObjectCreationInfos(Array<objectCreationInfo_t>& creationInfos);

private:
	Array<objectCreationInfo_t> m_objectCreationInfos;
};

extern ENGINE_API ObjectFactory* g_object_factory;

template<typename T>
inline void ObjectFactory::RegisterObject(const char* classname) {

	pfnCreateObject_t create_object_pfn = (pfnCreateObject_t)CreateObjectTemplated<T>;
	objectCreationInfo_t oci = {};
	oci.create_proc = create_object_pfn;
	oci.id = get_type_id<T>();
	oci.classname = classname;
	m_objectCreationInfos.push_back(oci);
}

template<typename T>
inline T* ObjectFactory::CreateObject()
{
	for (auto it : m_objectCreationInfos)
	{
		if (it.id == get_type_id<T>())
		{
			return (T*)it.create_proc();
		}
	}
	
	return NULL;
}

#define CREATE_OBJECT(CLASS, NAME) \
	(CLASS*)g_object_factory->CreateObjectByName(NAME)

#endif // !OBJECTFACTORY_H
