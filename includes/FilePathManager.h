#pragma once
#include <string>
#include "types.h"
class FilePathManager
{
public:
	FilePathManager(void);
public:
	~FilePathManager(void);
public:
	BOOL GetAbsolutePath(std::string strRelativePath, std::string& strAbsolutePath);
};
