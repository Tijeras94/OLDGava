#include "ObjectHeap.h"
#include "types.h"
#include "ClassHeap.h"

ObjectHeap::ObjectHeap(void)
:m_nNextObjectID(1)
{
}

ObjectHeap::~ObjectHeap(void)
{
}

Object ObjectHeap::CreateObject(JavaClass* pClass)
{
	Object object;	
	object.heapPtr=NULL;
	object.type=0;

	if(pClass == NULL) return object;
	u4 osize= pClass->GetObjectFieldCount()+1; //+1 for class pointer
	Variable *obj = new Variable[osize];
	if(!obj) return object;
	memset(obj, 0, sizeof(Variable)*osize);
	object.heapPtr=m_nNextObjectID++;
	obj[0].ptrValue =(LONG_PTR) pClass;

	m_ObjectMap.SetAt(object.heapPtr,obj);

	return object;
}

Object ObjectHeap::CreateStringObject(std::string pStrValue, ClassHeap *pClassHeap)
{
	Object object;	
	object.heapPtr=NULL;
	object.type=0;

	if(pClassHeap == NULL) return object;
	JavaClass *pClass=pClassHeap->GetClass(std::string("java/lang/String"));

	if(pClass == NULL) return object;	

	object = CreateObject(pClass);
	Variable *pVar= this->GetObjectPointer(object);
	if(pVar==NULL) return object;

	pVar[1].ptrValue=(LONG_PTR)((new std::string(pStrValue)));
	 
	return object;
}

BOOL ObjectHeap::CreateObjectArray(JavaClass* pClass, u4 count, Object& object)
{
	Variable *pVar= new Variable[count+1];

	if(pVar)
	{
		memset(pVar, 0, sizeof(Variable)*(count+1));
		pVar[0].ptrValue = (LONG_PTR)pClass;
	}
	else
		return FALSE;

	object.heapPtr=m_nNextObjectID++;
	
	m_ObjectMap.SetAt(object.heapPtr,pVar);

	return TRUE;
}

Variable* ObjectHeap::GetObjectPointer(Object object)
{
	Variable*obj = NULL;
	if(!m_ObjectMap.Lookup(object.heapPtr, &obj))
		return NULL;

	return obj;
}

Object ObjectHeap::CreateNewArray(u1 type, i4 count)
{
	Variable *obj= new Variable[count+1];
	Object object;
	object.heapPtr=0;
	object.type=type;

	if(obj)
	{
		memset(obj, 0, sizeof(Variable)*(count+1));
		object.heapPtr=m_nNextObjectID++;
		obj[0].intValue = type;

		m_ObjectMap.SetAt(object.heapPtr,obj);
	}

	return object;
}
