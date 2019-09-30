 #include "NativeMethods.h"
#include "ObjectHeap.h"
#include "ClassHeap.h"

Variable Add(RuntimeEnvironment* pRuntimeEnvironment)
{
	Frame* pFrameStack=pRuntimeEnvironment->pFrameStack;

	Variable returnVal;
	Frame *pFrame=&pFrameStack[0];
	returnVal.intValue= pFrame->pOpStack[pFrame->sp].intValue + pFrame->pOpStack[pFrame->sp-1].intValue;
	return returnVal;
}

Variable Print_N(RuntimeEnvironment* pRuntimeEnvironment)
{
	Variable returnVal;
	Frame* pFrame = &pRuntimeEnvironment->pFrameStack[0];
	int val = pFrame->stack[pFrame->sp].intValue;  
	printf("%i", val);
	returnVal.intValue = 0;
	return returnVal;
}

Variable Print(RuntimeEnvironment* pRuntimeEnvironment)
{
	Variable returnVal;
	Frame *pFrame=&pRuntimeEnvironment->pFrameStack[0];
	Object object=pFrame->stack[pFrame->sp].object;
	
 
	Variable *pVar=pRuntimeEnvironment->pObjectHeap->GetObjectPointer(object);
	if(pVar)
	{ 
		std::string* pString = ((std::string*)pVar[1].ptrValue);
		 
		if(pString)
			printf("%s",pString->c_str());
	}

	returnVal.intValue=0;
	return returnVal;	
}

Variable String_valueOf_F(RuntimeEnvironment* pRuntimeEnvironment)
{
	Variable returnVal;
	Frame *pFrame=&pRuntimeEnvironment->pFrameStack[0];
	ObjectHeap *pObjectHeap = pRuntimeEnvironment->pObjectHeap;
	ClassHeap *pClassHeap = pRuntimeEnvironment->pClassHeap;

	//static method
	f4 floatVal=pFrame->stack[0].floatValue;

	std::string str = std::to_string(floatVal); 
	returnVal.object=pRuntimeEnvironment->pObjectHeap->CreateStringObject(str, pRuntimeEnvironment->pClassHeap);			
	
	return returnVal;
}

Variable String_valueOf_J(RuntimeEnvironment* pRuntimeEnvironment)
{
	Variable returnVal;
	Frame *pFrame=&pRuntimeEnvironment->pFrameStack[0];
	ObjectHeap *pObjectHeap = pRuntimeEnvironment->pObjectHeap;
	ClassHeap *pClassHeap = pRuntimeEnvironment->pClassHeap;

	////static method
	f4 floatVal=pFrame->stack[0].floatValue;

	std::string str = std::to_string(floatVal); 
	returnVal.object=pRuntimeEnvironment->pObjectHeap->CreateStringObject(str, pRuntimeEnvironment->pClassHeap);			
	
	return returnVal;
}

//Method java/lang/StringBuilder.append(Ljava/lang/String;)Ljava/lang/StringBuilder;
Variable StringBuilder_append_String(RuntimeEnvironment* pRuntimeEnvironment)
{
	// we put a CString object as the first field in StringBuilder object

	Variable returnVal;
	Frame *pFrame=&pRuntimeEnvironment->pFrameStack[0];
	ObjectHeap *pObjectHeap = pRuntimeEnvironment->pObjectHeap;
	ClassHeap *pClassHeap = pRuntimeEnvironment->pClassHeap;
	Object thisObject=pFrame->stack[0].object;
	Object object=pFrame->stack[pFrame->sp].object;
	Variable *pVar=pRuntimeEnvironment->pObjectHeap->GetObjectPointer(object);
	Variable *pThisVar=pRuntimeEnvironment->pObjectHeap->GetObjectPointer(thisObject);

	if(pVar && pThisVar)
	{
		std::string *pString = (std::string *)pVar[1].ptrValue;
		std::string* pThisString = (std::string*)pThisVar[1].ptrValue;

		if(pThisVar[1].ptrValue==NULL)
		{
			pThisVar[1].ptrValue=(LONG_PTR) (pThisString= new std::string());
		}

		if(pThisString && pString)
		{
			//We now use...
			//pThisString->Append(pString->GetBuffer());
			*pString += *pString;

		}
	}
	
	returnVal.object=thisObject;

	return returnVal;
}

Variable StringBuilder_append_I(RuntimeEnvironment* pRuntimeEnvironment)
{
	Variable returnVal;
	Frame *pFrame=&pRuntimeEnvironment->pFrameStack[0];
	ObjectHeap *pObjectHeap = pRuntimeEnvironment->pObjectHeap;
	ClassHeap *pClassHeap = pRuntimeEnvironment->pClassHeap;
	Object thisObject=pFrame->stack[0].object;
	i4 intVal=pFrame->stack[pFrame->sp].intValue;
	Variable *pThisVar=pRuntimeEnvironment->pObjectHeap->GetObjectPointer(thisObject);

	if(pThisVar)
	{		
		std::string *pThisString = (std::string*)pThisVar[1].ptrValue;

		if(pThisVar[1].ptrValue==NULL)
		{
			pThisVar[1].ptrValue=(LONG_PTR) (pThisString=new std::string());
		}

		if(pThisString)
		{
			//We now use...
			std::string str = std::to_string(intVal);  
			*pThisString += str;
		}
	}
	
	returnVal.object=thisObject;

	return returnVal;
}

Variable StringBuilder_append_C(RuntimeEnvironment* pRuntimeEnvironment)
{
	Variable returnVal;
	Frame *pFrame=&pRuntimeEnvironment->pFrameStack[0];
	ObjectHeap *pObjectHeap = pRuntimeEnvironment->pObjectHeap;
	ClassHeap *pClassHeap = pRuntimeEnvironment->pClassHeap;
	Object thisObject=pFrame->stack[0].object;
	u2 charValue = pFrame->stack[pFrame->sp].shortValue;

	Variable *pThisVar=pRuntimeEnvironment->pObjectHeap->GetObjectPointer(thisObject);

	if(pThisVar)
	{		
		std::string *pThisString = (std::string*)pThisVar[1].ptrValue;

		if(pThisVar[1].ptrValue==NULL)
		{
			pThisVar[1].ptrValue=(LONG_PTR) (pThisString=new std::string());
		}

		if(pThisString)
		{
			//We now use...
			std::string str = std::to_string(charValue); 
			*pThisString += str;
		}
	}
	
	returnVal.object=thisObject;

	return returnVal;
}

Variable StringBuilder_append_Z(RuntimeEnvironment* pRuntimeEnvironment)
{
	Variable returnVal;
	Frame *pFrame=&pRuntimeEnvironment->pFrameStack[0];
	ObjectHeap *pObjectHeap = pRuntimeEnvironment->pObjectHeap;
	ClassHeap *pClassHeap = pRuntimeEnvironment->pClassHeap;
	Object thisObject=pFrame->stack[0].object;
	u4 boolValue = pFrame->stack[pFrame->sp].intValue;

	Variable *pThisVar=pRuntimeEnvironment->pObjectHeap->GetObjectPointer(thisObject);

	if(pThisVar)
	{		
		std::string *pThisString = (std::string*)pThisVar[1].ptrValue;

		if(pThisVar[1].ptrValue==NULL)
		{
			pThisVar[1].ptrValue=(LONG_PTR) (pThisString=new std::string());
		}

		if(pThisString)
		{
			//We now use...
			std::string str = std::string(boolValue? "true" : "false"); 
			*pThisString += str;
		}
	}
	
	returnVal.object=thisObject;

	return returnVal;

}

Variable StringBuilder_append_F(RuntimeEnvironment* pRuntimeEnvironment)
{
	Variable returnVal;
	Frame *pFrame=&pRuntimeEnvironment->pFrameStack[0];
	ObjectHeap *pObjectHeap = pRuntimeEnvironment->pObjectHeap;
	ClassHeap *pClassHeap = pRuntimeEnvironment->pClassHeap;
	Object thisObject=pFrame->stack[0].object;
	f4 floatValue = pFrame->stack[pFrame->sp].floatValue;

	Variable *pThisVar=pRuntimeEnvironment->pObjectHeap->GetObjectPointer(thisObject);

	if(pThisVar)
	{		
		std::string *pThisString = (std::string*)pThisVar[1].ptrValue;

		if(pThisVar[1].ptrValue==NULL)
		{
			pThisVar[1].ptrValue=(LONG_PTR) (pThisString=new std::string());
		}

		if(pThisString)
		{
			//We now use...
			std::string str = std::to_string(floatValue);
			//str.Format(_T("%f"), floatValue);
			*pThisString += str;// Append(str.GetBuffer());
		}
	}
	
	returnVal.object=thisObject;

	return returnVal;

}

//LONG
Variable StringBuilder_append_J(RuntimeEnvironment* pRuntimeEnvironment)
{
	Variable returnVal;
	Frame *pFrame=&pRuntimeEnvironment->pFrameStack[0];
	ObjectHeap *pObjectHeap = pRuntimeEnvironment->pObjectHeap;
	ClassHeap *pClassHeap = pRuntimeEnvironment->pClassHeap;
	Object thisObject=pFrame->stack[0].object;

	i4 i4High = pFrame->stack[pFrame->sp-1].intValue;
	i4 i4Low = pFrame->stack[pFrame->sp].intValue;

	i8 longValue= MAKEI8(i4High, i4Low);

	Variable *pThisVar=pRuntimeEnvironment->pObjectHeap->GetObjectPointer(thisObject);

	if(pThisVar)
	{		
		std::string *pThisString = (std::string*)pThisVar[1].ptrValue;

		if(pThisVar[1].ptrValue==NULL)
		{
			pThisVar[1].ptrValue=(LONG_PTR) (pThisString=new std::string());
		}

		if(pThisString)
		{
			//We now use...
			std::string str = std::to_string(longValue);
			//str.Format(_T("%I64d"), longValue);
			*pThisString += str;
		}
	}
	
	returnVal.object=thisObject;

	return returnVal;

}

//DOUBLE
Variable StringBuilder_append_D(RuntimeEnvironment* pRuntimeEnvironment)
{
	
	Variable returnVal;
	/*
	Frame *pFrame=&pRuntimeEnvironment->pFrameStack[0];
	ObjectHeap *pObjectHeap = pRuntimeEnvironment->pObjectHeap;
	ClassHeap *pClassHeap = pRuntimeEnvironment->pClassHeap;
	Object thisObject=pFrame->stack[0].object;
	
	f4 floatValue1 = pFrame->stack[pFrame->sp-1].floatValue;
	f4 floatValue2 = pFrame->stack[pFrame->sp].floatValue;

	f8 floatValue=0.0

	Variable *pThisVar=pRuntimeEnvironment->pObjectHeap->GetObjectPointer(thisObject);

	if(pThisVar)
	{		
		CString *pThisString = (CString *)pThisVar[1].ptrValue;

		if(pThisVar[1].ptrValue==NULL)
		{
			pThisVar[1].ptrValue=(LONG_PTR) (pThisString=new CString());
		}

		if(pThisString)
		{
			//We now use...
			CString str;
			str.Format(_T("%f"), floatValue);
			pThisString->Append(str.GetBuffer());
		}
	}
	
	returnVal.object=thisObject;
	*/
	returnVal.floatValue=0;

	return returnVal;

}


Variable StringBuilder_append_Object(RuntimeEnvironment* pRuntimeEnvironment)
{
	Variable returnVal;
	Frame *pFrame=&pRuntimeEnvironment->pFrameStack[0];
	ObjectHeap *pObjectHeap = pRuntimeEnvironment->pObjectHeap;
	ClassHeap *pClassHeap = pRuntimeEnvironment->pClassHeap;
	Object thisObject=pFrame->stack[0].object;
	Object object=pFrame->stack[pFrame->sp].object;
	Variable *pVar=pRuntimeEnvironment->pObjectHeap->GetObjectPointer(object);
	Variable *pThisVar=pRuntimeEnvironment->pObjectHeap->GetObjectPointer(thisObject);

	
	// TODO: call objects toString and append
	if(pThisVar)
	{
		
		std::string *pThisString = (std::string*)pThisVar[1].ptrValue;

		std::string str;
		if(pVar)
		{
			JavaClass *pClass = (JavaClass *)pVar[0].ptrValue;		
			//str.Format(_T("Object [%X] of Class %ws"),object.heapPtr, pClass->GetName());
			char buff[255];
			snprintf(buff, sizeof(buff), "Object [%X] of Class %ws", pClass->GetName());
			str = std::string(buff);
		}
		else
			str= "null";


		if(pThisVar[1].ptrValue==NULL)
		{
			pThisVar[1].ptrValue = (LONG_PTR)(pThisString=new std::string());
		}
		if(pThisString)
		{
			*pThisString += str;
		}
	}
	
	
	returnVal.object=thisObject;

	return returnVal;
}

Variable StringBuilder_toString_String(RuntimeEnvironment* pRuntimeEnvironment)
{
	// we put a CString object as the first field in StringBuilder object

	Variable returnVal;
	returnVal.object.heapPtr=NULL;

	Frame *pFrame=&pRuntimeEnvironment->pFrameStack[0];
	ObjectHeap *pObjectHeap = pRuntimeEnvironment->pObjectHeap;
	ClassHeap *pClassHeap = pRuntimeEnvironment->pClassHeap;
	Object thisObject=pFrame->stack[0].object;

	Variable *pThisVar=pRuntimeEnvironment->pObjectHeap->GetObjectPointer(thisObject);

	if(pThisVar)
	{
		std::string *pThisString = (std::string*)pThisVar[1].ptrValue;

		if(pThisVar[1].ptrValue==NULL)
		{
			pThisVar[1].ptrValue=(LONG_PTR) (pThisString=new std::string());
		}

		if(pThisString)
		{
			returnVal.object=pRuntimeEnvironment->pObjectHeap->CreateStringObject(*pThisString, pRuntimeEnvironment->pClassHeap);
		}
	}
	
	return returnVal;
}