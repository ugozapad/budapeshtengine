#ifndef OBJECT_H
#define OBJECT_H

// Simple run time type information, inspired by TurboBadger and Oryol

// Disable GetClassName macro from Win32 API
#ifdef WIN32
#undef GetClassName
#endif

typedef void* typeId_t;

//! Base class for run-time type info.
class ENGINE_API TypedObject {
public:
	virtual ~TypedObject() {}

	//! Get object class id.
	template <typename T> static typeId_t GetTypeId() { static char typeId; return &typeId; }

	//! Get class name.
	virtual const char* GetClassName() { return "TypedObject"; }

	//! Match type with another class.
	virtual bool IsTypeOf(const typeId_t typeId) const { return typeId == GetTypeId<TypedObject>(); }

	//! Templated version of isKindOf.
	template <typename T> bool IsKindOf() const { return IsTypeOf(GetTypeId<T>()); }
};

//! Safe dynamic cast for all TypedObject
template <typename T>
T* dynamicCast(TypedObject* object) {
	if (object->IsKindOf<T>())
		return (T*)object;

	return nullptr;
}

//! Get type id from TypedObject
template <typename T>
typeId_t get_type_id() {
	return TypedObject::GetTypeId<T>();
}

//! Define for subclass of TypedObject
#define OBJECT_IMPLEMENT(CLASS, BASECLASS) \
	virtual const char* GetClassName() { return #CLASS; } \
	virtual bool IsTypeOf(const typeId_t typeId) const { return GetTypeId<CLASS>() == typeId ? true : BASECLASS::IsTypeOf(typeId);  }

//! Define for easy using isKindOf for subclasses of TypedObject
#define ObjectIsKindOf(PTR, CLASS) \
	PTR->isTypeOf(get_type_id<CLASS>())

#endif