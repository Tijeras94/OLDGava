#pragma once
#include <string>
#include "Cmap.h"

class JavaClass;
class FilePathManager;

class ClassHeap
{
	CMap<std::string, JavaClass> m_ClassMap;
	FilePathManager *pFilePathManager;
public:
	ClassHeap(void);
public:
	virtual ~ClassHeap(void);
public:
	BOOL AddClass(JavaClass* pJavaClass);
	JavaClass* GetClass(std::string strClassName);
	BOOL LoadClass(std::string strClassName, JavaClass *pClass);


};
