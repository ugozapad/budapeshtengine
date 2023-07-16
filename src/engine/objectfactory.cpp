#include "pch.h"
#include "engine/objectfactory.h"

ObjectFactory* g_object_factory = nullptr;

ObjectFactory::ObjectFactory(IAllocator& allocator) :
	m_allocator(allocator),
	m_objectCreationInfos(allocator) {
}

ObjectFactory::~ObjectFactory() {
}
