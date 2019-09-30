
#include "ExecutionEngine.h"
#include "JavaClass.h"
#include "types.h"
#include "opcodes.h"
#include "ClassHeap.h"
#include "ObjectHeap.h"
#include <assert.h>
#include "NativeMethods.h" 


Variable* Frame::pOpStack; //static
Frame* Frame::pBaseFrame;

ExecutionEngine::ExecutionEngine(void)
:	pClassHeap(NULL),
	pObjectHeap(NULL)
{	
}

ExecutionEngine::~ExecutionEngine(void)
{
}
  
u4 ExecutionEngine::Execute(Frame* pFrameStack)
{
	assert(pFrameStack);
	Frame* pFrame=&pFrameStack[0];
	assert(pFrame);

#ifdef DBG_PRINT
	DbgPrint(("Current Frame %ld Stack start at %ld\n"),pFrame-Frame::pBaseFrame, pFrame->stack-Frame::pOpStack );
#endif
	printf("Current Frame %ld Stack start at %ld\n", pFrame - Frame::pBaseFrame, pFrame->stack - Frame::pOpStack);


	if(pFrame->pMethod->access_flags & ACC_NATIVE)
	{

		DbgPrint("Enter Native Method\n");

		ExecuteNativeMethod(pFrame);

		DbgPrint("Exit Native Method\n");

		return 0;
	}
	 
	return pFrame->exec(this);
}

void ExecutionEngine::ExecuteInvokeSpecial(Frame* pFrameStack)
{
	ExecuteInvokeVirtual(pFrameStack, invokespecial);
}

Variable ExecutionEngine::LoadConstant(JavaClass *pClass, u1 nIndex)
{
	Variable v;
	v.ptrValue = 0;
	std::string pStrVal = "";
	std::string strTemp = "";
	//std::string strClass= pClass->GetName();
	//ShowClassInfo(pClass);

	char* cp=(char *)pClass->constant_pool[nIndex];
	u2 i;
	Object object;
	switch(cp[0])
	{
	case CONSTANT_Integer:
		v.intValue = getu4(&cp[1]);
		break;

	case CONSTANT_Float:
		v.floatValue = getf4((char *)&cp[1]);
		break;

	case CONSTANT_String:
		i=getu2((char *)&cp[1]);

		pStrVal = "";
		pClass->GetStringFromConstPool(i, strTemp);	
		pStrVal = pStrVal + strTemp;
		object = this->pObjectHeap->CreateStringObject(pStrVal, pClassHeap);
		v.ptrValue=object.heapPtr;
		break;

	case CONSTANT_Double:
		break;

	case CONSTANT_Long:

		break;		
	}
	return v;
}
void ExecutionEngine::PutField(Frame* pFrameStack)
{
	u2 nIndex = getu2(&pFrameStack[0].pMethod->pCode_attr->code[pFrameStack[0].pc+1]);
	Variable obj=pFrameStack[0].stack[pFrameStack[0].sp-1];
	Variable value=pFrameStack[0].stack[pFrameStack[0].sp];
	Variable *pVarList=this->pObjectHeap->GetObjectPointer(obj.object);
	JavaClass *pClass = (JavaClass *)pVarList[0].ptrValue;
	assert(pClass && pClass->magic == 0xCAFEBABE);
	//ShowClassInfo(pClass);
	pVarList[nIndex+1]=value;
}

void ExecutionEngine::GetField(Frame* pFrame)
{
	u2 nIndex = getu2(&pFrame->pMethod->pCode_attr->code[pFrame->pc+1]);
	Variable obj=pFrame->stack[pFrame->sp];
	
	Variable *pVarList=this->pObjectHeap->GetObjectPointer(obj.object);
	JavaClass *pClass = (JavaClass *)pVarList[0].ptrValue;
	assert(pClass && pClass->magic == 0xCAFEBABE);
	//ShowClassInfo(pClass);
	pFrame->stack[pFrame->sp]=pVarList[nIndex+1];
}

void ExecutionEngine::ExecuteInvokeVirtual(Frame* pFrameStack, u2 type)
{
	u2 mi=getu2(&pFrameStack[0].pMethod->pCode_attr->code[pFrameStack[0].pc+1]);
	Variable objectRef = pFrameStack[0].stack[pFrameStack[0].sp]; 
	char *pConstPool = (char *)pFrameStack[0].pClass->constant_pool[mi];

	assert(pConstPool[0] == CONSTANT_Methodref);
		
	u2 classIndex = getu2(&pConstPool[1]);
	u2 nameAndTypeIndex = getu2(&pConstPool[3]);

	//get class at pool index 
	pConstPool = (char *)pFrameStack[0].pClass->constant_pool[classIndex];

	assert(pConstPool[0] == CONSTANT_Class);

	u2 ni=getu2(&pConstPool[1]);

	std::string strClassName;
	pFrameStack[0].pClass->GetStringFromConstPool(ni, strClassName);


	JavaClass *pClass=pClassHeap->GetClass(strClassName);

	//ShowClassInfo(pClass);

	pConstPool = (char *)pFrameStack[0].pClass->constant_pool[nameAndTypeIndex];


	assert(pConstPool[0] == CONSTANT_NameAndType);


	method_info_ex method;

	
	method.name_index = getu2(&pConstPool[1]);
	method.descriptor_index = getu2(&pConstPool[3]);

	method.access_flags = 0; // todo set 

	std::string strName, strDesc;
	pFrameStack[0].pClass->GetStringFromConstPool(method.name_index, strName);
	pFrameStack[0].pClass->GetStringFromConstPool(method.descriptor_index, strDesc);

	
	//printf("SuperClass - %s",(method.access_flags& ACC_SUPER)?"Yes":"No");
	JavaClass *pVirtualClass=pClass;
	int nIndex=pClass->GetMethodIndex(strName, strDesc, pVirtualClass);

	memset(&pFrameStack[1],0,sizeof(pFrameStack[1]));
	pFrameStack[1].pMethod = &pClass->methods[nIndex];
		
	method.access_flags = getu2((char *)pFrameStack[1].pMethod);
	if( ACC_SUPER & method.access_flags)
	{
		pFrameStack[1].pClass = pVirtualClass->GetSuperClass();
		//ShowClassInfo(pFrameStack[1].pClass);
	}
	else
	{
		pFrameStack[1].pClass=pVirtualClass;
	}

	//pFrameStack[1].pOpStack[++pFrameStack[1].sp]=pFrameStack[0].pOpStack[pFrameStack[0].sp--];
	int params=GetMethodParametersStackCount(strDesc)+1;
	
	//static
	if(type==invokestatic) params--;

	int nDiscardStack =params;
	if(pFrameStack[1].pMethod->access_flags & ACC_NATIVE)
	{
	}
	else 
	{
		nDiscardStack+=pFrameStack[1].pMethod->pCode_attr->max_locals; 
	}
	
	pFrameStack[1].stack = &Frame::pOpStack[pFrameStack->stack-Frame::pOpStack+pFrameStack[0].sp-params+1];
	pFrameStack[1].sp=nDiscardStack-1;
	//DbgPrint(("Invoking method %s%s, \n"), strName, strDesc);
	//DbgPrint(("Last Frame Stack %d Params %d Stack start at %d\n"),pFrameStack[0].stack-Frame::pOpStack+pFrameStack[0].sp,pFrameStack[1].sp,pFrameStack[1].stack-Frame::pOpStack );

	this->Execute(&pFrameStack[1]);

	//if returns then get on stack	
	
	if(strDesc.find((")V")) > 0)
	{
		nDiscardStack--;		
	}

	pFrameStack[0].sp-=nDiscardStack;
	pFrameStack[0].stack[pFrameStack[0].sp] = pFrameStack[1].stack[0];

}

u2 ExecutionEngine::GetMethodParametersCount(std::string strMethodDesc)
{
	u2 count=0;
	
	int i, len = strMethodDesc.length();

	//todo: long/double takes 2 stack position
	for(i=1;i<len;i++)
	{
		if(strMethodDesc[i] =='L')
		{
			while(strMethodDesc[i] !=';') i++;
		}
		if(strMethodDesc[i] ==')') break;
		count++;
	}

	return count;
}

u2 ExecutionEngine::GetMethodParametersStackCount(std::string strMethodDesc)
{
	u2 count=0;
	
	int i, len=strMethodDesc.length();

	//todo: long/double takes 2 stack position
	for(i=1;i<len;i++)
	{
		if(strMethodDesc[i] =='L')
		{
			while(strMethodDesc[i] !=';') i++;
		}
		if(strMethodDesc[i] ==')') break;
		if(strMethodDesc[i] =='J' || strMethodDesc[i] =='D')
			count++;
		count++;
	}

	return count;
}

pNativeMethod GetNativeMethod(std::string strSign)
{
	
	if(FALSE)
	{
	}
	else if(!strSign.compare(("java/lang/String@valueOf(F)Ljava/lang/String;")))
	{
		return String_valueOf_F;
	}
	else if(!strSign.compare(("java/lang/String@valueOf(J)Ljava/lang/String;")))
	{
		return String_valueOf_J;
	}
	else if(!strSign.compare(("java/lang/StringBuilder@append(Ljava/lang/String;)Ljava/lang/StringBuilder;")))
	{
		return StringBuilder_append_String;
	}
	else if(!strSign.compare(("java/lang/StringBuilder@append(I)Ljava/lang/StringBuilder;")))
	{
		return StringBuilder_append_I;
	}
	else if(!strSign.compare(("java/lang/StringBuilder@append(C)Ljava/lang/StringBuilder;")))
	{
		return StringBuilder_append_C;
	}
	else if(!strSign.compare(("java/lang/StringBuilder@append(Z)Ljava/lang/StringBuilder;")))
	{
		return StringBuilder_append_Z;
	}
	else if(!strSign.compare(("java/lang/StringBuilder@append(Ljava/lang/Object;)Ljava/lang/StringBuilder;")))
	{
		return StringBuilder_append_Object;
	}
	else if(!strSign.compare(("java/lang/StringBuilder@append(F)Ljava/lang/StringBuilder;")))
	{
		return StringBuilder_append_F;
	}
	else if(!strSign.compare(("java/lang/StringBuilder@append(J)Ljava/lang/StringBuilder;")))
	{
		return StringBuilder_append_J;
	}

	
	
	else if(!strSign.compare(("java/lang/StringBuilder@toString()Ljava/lang/String;")))
	{
		return StringBuilder_toString_String;
	}
	else if(!strSign.compare(("Test@Print(Ljava/lang/String;)V")))
	{
		return Print;
	}
	else if (!strSign.compare(("Test@Print(I)V")))
	{
		return Print_N;
	}

	return NULL;
}

u4 ExecutionEngine::ExecuteNativeMethod(Frame* pFrameStack)
{
	assert(pFrameStack);
	Frame* pFrame=&pFrameStack[0];
	assert(pFrame->pMethod->access_flags & ACC_NATIVE);

	JavaClass *pClass = pFrame->pClass;
	std::string strClassName, strMethod, strDesc, strSignature;
	strClassName=pClass->GetName();
	pClass->GetStringFromConstPool(pFrame->pMethod->name_index, strMethod);
	pClass->GetStringFromConstPool(pFrame->pMethod->descriptor_index, strDesc);
	
	//DbgPrint(("Execute At Class %s Method %s%s  \n"),strClassName , strMethod, strDesc);
	
	strSignature=strClassName+("@")+strMethod+strDesc;
	pNativeMethod pNativeMethod=GetNativeMethod(strSignature);
	RuntimeEnvironment rte;
	rte.pFrameStack=pFrameStack;
	rte.pClassHeap= pClassHeap;
	rte.pObjectHeap= pObjectHeap;

	if(pNativeMethod == NULL)
	{
		// what should I do here??
		// System Panic

		assert(FALSE);
	}
	else
	{
		Variable retVal = pNativeMethod(&rte);

		//if returns then get on stack	
		if(strDesc.find((")V")) < 0)
		{
			//todo validate
			pFrame->stack[0]=retVal;
		}
	}
	 
	return 0;
}

int ExecutionEngine::ExecuteNew(Frame* pFrame)
{
	pFrame->sp++;
	u1 *bc=pFrame->pMethod->pCode_attr->code;
	u2 index=getu2(&bc[pFrame->pc+1]);

	if(!pFrame->pClass->CreateObject(index, this->pObjectHeap, pFrame->stack[pFrame->sp].object))
		return -1;
	

	return 0;
}

int ExecutionEngine::ExecuteNewArray(Frame* pFrame)
{
	u1 type=pFrame->pMethod->pCode_attr->code[pFrame->pc+1];
	pFrame->stack[pFrame->sp+1].object = pObjectHeap->CreateNewArray(type, pFrame->stack[pFrame->sp].intValue);
	pFrame->sp++;
	return 0;
}
int ExecutionEngine::ExecuteANewArray(Frame* pFrame)
{
	u2 index=getu2(&pFrame->pMethod->pCode_attr->code[pFrame->pc+1]);
	u4 count=pFrame->stack[pFrame->sp].intValue;
	pFrame->sp++;

	//be carefull abuot stack pointer - its incremented once
	if(!pFrame->pClass->CreateObjectArray(index, count,pObjectHeap, pFrame->stack[pFrame->sp].object))
	{
		return -1;
	}
	
	return 0;
}

Variable* ExecutionEngine::CreateNewArray(u1 type, i4 count)
{
	Variable *pVar= new Variable[count+1];

	if(pVar)
	{
		memset(pVar, 0, sizeof(Variable)*(count+1));
		pVar[0].intValue = type;
	}

	return pVar;
}

void ExecutionEngine::DumpObject(Object object)
{
	return;
#ifdef DEBUG
	JavaClass *cls=(JavaClass *)this->pObjectHeap->GetObjectPointer(object)[0].ptrValue;
	if((LONG_PTR)cls < 15)
	{
		return;
	}

	DbgPrint(("Load Class %s\n"),cls->GetName());
#endif
}
