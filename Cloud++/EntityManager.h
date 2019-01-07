#include "Cloud.h"
#pragma once

class EntityManager
{
public:
	static void Cache();
	static std::list<GameObject*> EntityChampions;
	static std::list<GameObject*> EntityMinions;
	static std::list<GameObject*> EntityTurrets;
};