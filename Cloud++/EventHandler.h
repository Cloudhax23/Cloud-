#include "stdafx.h"
#include <map>
#pragma once

class EventHandler
{
public:
	static std::multimap<char*, void*> EventsMap;
	static void AddEventHandler(char*, void*);
	template<typename T, typename ... Args >
	static void TriggerEventHandler(char* name, Args ... args)
	{
		if (EventsMap.count(name) == 0)
		{
			return;
		}
		else
		{
			std::pair<std::multimap<char*, void*>::iterator, std::multimap<char*, void*>::iterator> mapIterator;
			mapIterator = EventsMap.equal_range(name);
			for (std::multimap<char*, void*>::iterator eventIterator = mapIterator.first; eventIterator != mapIterator.second; ++eventIterator)
			{
				((T*)((*eventIterator).second)) (args...);
			}
		}
	};
};