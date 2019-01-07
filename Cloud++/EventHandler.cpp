#include "stdafx.h"
#include "EventHandler.h"

std::multimap<char*, void*> EventHandler::EventsMap;

void EventHandler::AddEventHandler(char* name, void* func)
{
	EventsMap.insert(std::pair<char*, void*>(name, func));
}