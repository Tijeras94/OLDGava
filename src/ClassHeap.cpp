#include "ClassHeap.h"
#include "JavaClass.h"
#include "FilePathManager.h"
ClassHeap::ClassHeap(void)
{
	pFilePathManager = new FilePathManager();
}

ClassHeap::~ClassHeap(void)
{
	
}

BOOL ClassHeap::AddClass(JavaClass* pJavaClass)
{
	if(!pJavaClass) return FALSE;
	std::string name=pJavaClass->GetName();

	//TODO- check if already exists

	m_ClassMap.SetAt(name, pJavaClass);
	return TRUE;
}

JavaClass* ClassHeap::GetClass(std::string strClassName)
{
	JavaClass*pClass=NULL;
	if(!m_ClassMap.Lookup(strClassName, &pClass))
	{
		pClass = new JavaClass();
		BOOL bRet=this->LoadClass(strClassName, pClass);
		if(!bRet)
		{
			delete pClass;
			pClass=NULL;
		}		
	}

	return (JavaClass*)pClass;
}

BOOL ClassHeap::LoadClass(std::string strClassName, JavaClass *pClass)
{
	std::string path, relPath;
	if(!pClass) return FALSE;
	relPath=strClassName+".class";
	if(!pFilePathManager->GetAbsolutePath(relPath, path))
		return FALSE;	

	BOOL bRet=pClass->LoadClassFromFile(path);

	if (!bRet) {
		//printf("Unable to open %s\n", relPath.c_str());
		return FALSE;
	} 

	pClass->SetClassHeap(this);

	return AddClass(pClass);
}
