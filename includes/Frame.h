#pragma once
#include "types.h"

class ExecutionEngine;
class Frame
{
public:
	static Variable* pOpStack;
	static Frame* pBaseFrame;
	JavaClass* pClass;
	method_info_ex* pMethod;
	u4 pc;
	i2 sp;
	Variable* stack;
	Frame()
	{
		sp = -1;
		pClass = NULL;
		pc = 0;
		stack = NULL;
		pBaseFrame = NULL;
		pOpStack = NULL;
	}

	Frame(i2 sp)
	{
		this->sp = sp;
		pClass = NULL;
		pc = 0;
		stack = NULL;
	}

	int exec(ExecutionEngine* engine);
};