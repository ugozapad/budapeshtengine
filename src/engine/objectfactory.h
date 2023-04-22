#ifndef OBJECTFACTORY_H
#define OBJECTFACTORY_H

#include "engine/array.h"
#include "engine/object.h"

class TypedObject;
typedef TypedObject* (*pfnCreateObject_t)(IAllocator& allocator);

template <typename T>
T* createObjectTemplated(IAllocator& allocator) {
	return MEM_NEW(allocator, T);
}

struct objectCreationInfo_t {
	typeId_t id;
	pfnCreateObject_t create_proc;
};

class ObjectFactory {
public:
	ObjectFactory(IAllocator& allocator);
	~ObjectFactory();

	template <typename T>
	void registerObject();

	template <typename T>
	T* createObject(IAllocator& allocator);

private:
	Array<objectCreationInfo_t> m_objectCreationInfos;
	IAllocator& m_allocator;
};

extern ObjectFactory* g_object_factory;

template<typename T>
inline void ObjectFactory::registerObject() {

	pfnCreateObject_t create_object_pfn = (pfnCreateObject_t)createObjectTemplated<T>;
	objectCreationInfo_t object_creation_info = {};
	object_creation_info.create_proc = create_object_pfn;
	object_creation_info.id = get_type_id<T>();
	m_objectCreationInfos.push_back(object_creation_info);
}

template<typename T>
inline T* ObjectFactory::createObject(IAllocator& allocator) {
	for (auto it : m_objectCreationInfos) {
		if (it.id == get_type_id<T>()) {
			return (T*)it.create_proc(allocator);
		}
	}
	
	return NULL;
}

#endif // !OBJECTFACTORY_H
