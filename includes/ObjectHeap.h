#pragma once
#include <string>
#include "CMap.h"

class ObjectHeap
{
public:
	ObjectHeap(void);
public:
	virtual ~ObjectHeap(void);
public:
	virtual Variable* GetObjectPointer(Object object);
	Object CreateObject(JavaClass* pClass);
	Object CreateStringObject(std::string pStrValue, ClassHeap *pClassHeap);
	Object CreateNewArray(u1 type, i4 count);
	BOOL CreateObjectArray(JavaClass* pClass, u4 count, Object& object);
private:
	CMap<LONG_PTR, Variable> m_ObjectMap;
	u4 m_nNextObjectID;

};
