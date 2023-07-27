#ifndef OBJECT_H
#define OBJECT_H

// Simple run time type information, inspired by TurboBadger and Oryol

typedef void* typeId_t;

//! Base class for run-time type info.
class ENGINE_API TypedObject {
public:
	virtual ~TypedObject() {}

	//! Get object class id.
	template <typename T> static typeId_t getTypeId() { static char typeId; return &typeId; }

	//! Get class name.
	virtual const char* getClassName() { return "TypedObject"; }

	//! Match type with another class.
	virtual bool isKindOf(const typeId_t typeId) const { return typeId == getTypeId<TypedObject>(); }

	//! Templated version of isKindOf.
	template <typename T> bool isKindOf() const { return isKindOf(getTypeId<T>()); }
};

//! Safe dynamic cast for all TypedObject
template <typename T>
T* dynamicCast(TypedObject* object) {
	if (object->isKindOf<T>())
		return (T*)object;

	return nullptr;
}

//! Get type id from TypedObject
template <typename T>
typeId_t get_type_id() {
	return TypedObject::getTypeId<T>();
}

//! Define for subclass of TypedObject
#define OBJECT_IMPLEMENT(CLASS, BASECLASS) \
	virtual const char* getClassName() { return #CLASS; } \
	virtual bool isKindOf(const typeId_t typeId) const { return getTypeId<CLASS>() == typeId ? true : BASECLASS::isKindOf(typeId);  }

//! Define for easy using isKindOf for subclasses of TypedObject
#define ObjectIsKindOf(PTR, CLASS) \
	PTR->isKindOf(get_type_id<CLASS>())

#endif