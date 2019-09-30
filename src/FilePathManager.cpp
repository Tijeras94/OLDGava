#include "FilePathManager.h" 

FilePathManager::FilePathManager(void)
{
}

FilePathManager::~FilePathManager(void)
{
}

BOOL FilePathManager::GetAbsolutePath(std::string strRelativePath, std::string& strAbsolutePath)
{
	//TCHAR path[MAX_PATH+1];

	//GetCurrentDirectory(MAX_PATH, path);

	//strAbsolutePath = path+(_T("\\")+strRelativePath);

	//if(!PathFileExists(strAbsolutePath)) 
	//	return FALSE;

	strAbsolutePath = "..//gdk//" + strRelativePath;

	return TRUE;
}
