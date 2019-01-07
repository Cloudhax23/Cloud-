#include "stdafx.h"
#include "EntityManager.h"
#include "Cloud.h"

std::list<GameObject*> EntityManager::EntityChampions;
std::list<GameObject*> EntityManager::EntityMinions;
std::list<GameObject*> EntityManager::EntityTurrets;

void EntityManager::Cache()
{
	EntityChampions.clear();
	EntityMinions.clear();
	EntityTurrets.clear();
	for (auto i = 0; i < 10000; i++)
	{
		GameObject* gameObj = ObjectManager().GetObjectArray()[i];
		if (Cloud::isValidAddress(gameObj))
		{
			if (gameObj->Type->Name.operator std::string() == std::string("AITurretClient"))
				EntityTurrets.push_back(gameObj);
			if (gameObj->Type->Name.operator std::string() == std::string("AIMinionClient"))
				EntityMinions.push_back(gameObj);
			if (gameObj->Type->Name.operator std::string() == std::string("AIHeroClient"))
				EntityChampions.push_back(gameObj);
		}
	}
}