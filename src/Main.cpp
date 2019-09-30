#include <stdio.h>
#include "constants.h"
#include "types.h"

#include "Frame.h"
#include "ClassHeap.h"
#include "ObjectHeap.h"
#include "ExecutionEngine.h"
 
int main() {
	ClassHeap heap;
	JavaClass* jclass = new JavaClass();
	heap.LoadClass("Test", jclass);
	
	for (size_t i = 1; i < jclass->constant_pool_count-1; i++)
	{
		cp_info inf;
		jclass->GetConstantPool(i, inf);

		std::string tg;
		getContantTagName(inf, tg);
		printf("%s(%i): ", tg.c_str(), (int)i);

		if (inf.tag == CONSTANT_String) {  
			std::string name;
			jclass->GetStringFromConstPool(i, name);
			printf("%s", name.c_str());
		} else if (inf.tag == CONSTANT_Utf8) { 
			std::string name;
			jclass->GetStringFromConstPool(i, name);
			printf("%s", name.c_str());
		}
		printf("\n");

	}


	return 0;
}

int done() {
	ClassHeap heap;
	JavaClass* pClass1 = new JavaClass();
	JavaClass* pClass2 = new JavaClass();

	BOOL bRet = heap.LoadClass("Test", pClass1); 
	bRet = heap.LoadClass("java/lang/Object", pClass2);

	ObjectHeap oheap;
	Object obj = oheap.CreateObject(pClass1); 


	Frame* pFrameStack = new Frame[20];
	Frame::pBaseFrame = pFrameStack;

	memset(pFrameStack, 0, sizeof(Frame) * 20);

	Frame::pOpStack = new Variable[100];
	memset(Frame::pOpStack, 0, sizeof(Variable) * 100);

	ExecutionEngine ex;
	ex.pClassHeap = &heap;
	ex.pObjectHeap = &oheap;
	int startFrame = 0;

	Object object = oheap.CreateObject(pClass1);
	JavaClass* pVirtualClass = pClass1;
	int mindex = pClass1->GetMethodIndex("Entry", "()I", pVirtualClass);
	pFrameStack[startFrame].pClass = pVirtualClass; 
	pFrameStack[startFrame].pMethod = &pVirtualClass->methods[mindex];
	pFrameStack[startFrame].stack = Frame::pOpStack;
	pFrameStack[startFrame].sp = pFrameStack[startFrame].pMethod->pCode_attr->max_locals;

	//Frame frm = pFrameStack[startFrame];
	//frm.stack[0] = object
	pFrameStack[startFrame].stack[0].object = object;

	// Frame frm = pFrameStack[startFrame]
	// ex.Execute(&frm)
	ex.Execute(&pFrameStack[startFrame]);
	return 0;
}