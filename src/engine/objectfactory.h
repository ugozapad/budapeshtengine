#ifndef OBJECTFACTORY_H
#define OBJECTFACTORY_H

#include "engine/array.h"
#include "engine/object.h"

class TypedObject;
typedef TypedObject* (*pfnCreateObject_t)();

template <typename T>
T* createObjectTemplated() {
	return new T();
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
	void registerObject(const char* classname);

	template <typename T>
	T* createObject();

	TypedObject* createByName(const char* classname);

private:
	Array<objectCreationInfo_t> m_objectCreationInfos;
};

extern ENGINE_API ObjectFactory* g_object_factory;

template<typename T>
inline void ObjectFactory::registerObject(const char* classname) {

	pfnCreateObject_t create_object_pfn = (pfnCreateObject_t)createObjectTemplated<T>;
	objectCreationInfo_t oci = {};
	oci.create_proc = create_object_pfn;
	oci.id = get_type_id<T>();
	oci.classname = classname;
	m_objectCreationInfos.push_back(oci);
}

template<typename T>
inline T* ObjectFactory::createObject()
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

#endif // !OBJECTFACTORY_H
