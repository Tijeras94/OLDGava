#pragma once
#include "types.h"
#include "JavaClass.h"
#include <map>

template<typename K , typename V>
class CMap {
public: 
	BOOL SetAt(K key, V* data)
	{ 
		if (map.count(key) > 0)
		{
			return FALSE;
		}
		else
			map[key] = data;

		return TRUE;

	}
	BOOL Lookup(K key, V** data)
	{
		if (map.count(key) > 0)
		{ 
			*data = (V*)map[key];
			return TRUE;
		}else
			return FALSE;
	}

private:
	std::map<K, V*> map;
};