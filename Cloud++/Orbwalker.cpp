#include "stdafx.h"
#include "Orbwalker.h"
#include "EventHandler.h"
#include "EntityManager.h"
#include "Drawing.h"
#include <algorithm>
#include "cpplinq.hpp"

std::map<GameObject*, PredictedDamage*> Orbwalker::ActiveAttacks = std::map<GameObject*, PredictedDamage*>();
int Orbwalker::lastAttackCommand = 0;
int Orbwalker::lastOrderCommand = 0;
int Orbwalker::LastAATick = 0;
bool Orbwalker::_missileLaunched = false;
GameObject* Orbwalker::player = ObjectManager::GetPlayer();
#define Game_Ping 45

Orbwalker::Orbwalker()
{
	EventHandler::AddEventHandler("OnProcessSpell", ProcessSpell);
	EventHandler::AddEventHandler("OnRenderHUD", OnTick);
	EventHandler::AddEventHandler("OnCreateObject", CreateObj);
	EventHandler::AddEventHandler("OnDeleteObject", DeleteObj);
}


Orbwalker::~Orbwalker()
{
}

void Orbwalker::CreateObj(GameObject* minion)
{
	if (minion->Type->Name.operator std::string() == std::string("MissileClient"))
	{
		if (minion->missile.SenderHandle == player->Handle && minion->missile.isAutoAttack)
			_missileLaunched = true;
	}
}

void Orbwalker::DeleteObj(GameObject* minion)
{
	if (minion->Team != 0)
	{
		if (minion->Type->Name.operator std::string() == std::string("MissileClient"))
		{
			GameObject* sender = ObjectManager::GetObjectArray()[minion->missile.SenderHandle];
			if (ActiveAttacks.count(sender) != 0)
			{
				ActiveAttacks[sender]->Processed = true;
			}
		}
	}
}

OrbwalkingMode Orbwalker::Mode()
{
	if (GetAsyncKeyState(VK_SPACE))
		return OrbwalkingMode::Combo;
	if (GetAsyncKeyState(0x56))
		return OrbwalkingMode::LaneClear;
	if (GetAsyncKeyState(0x58))
		return OrbwalkingMode::LastHit;
	if (GetAsyncKeyState(0x43))
		return OrbwalkingMode::Mixed;
	return OrbwalkingMode::None;
}

void Orbwalker::OnTick(int b)
{
	for (auto attackData : cpplinq::from(ActiveAttacks) >> cpplinq::where([](std::pair<GameObject*, PredictedDamage*> Obj) {return Obj.second->initateTime < GetTickCount() - 3000; }) >> cpplinq::to_list())
	{
		ActiveAttacks.erase(attackData.first);
	}
	if(Mode() != OrbwalkingMode::None)
		Orbwalk(GetTarget(), Cursor::GetInstance()->cursorPos, 90, 100);
}
void Orbwalker::Orbwalk(GameObject* target, D3DXVECTOR3 pos, int windUp, int holdRadius)
{
	if (GetTickCount() - lastAttackCommand < 70 + std::min(60, Game_Ping))
		return;

	if (target->isValid())
	{
		if (player->isInAARange(target) && CanAttack())
		{
			_missileLaunched = false;
			lastAttackCommand = GetTickCount();
			player->IssueOrder(&target->Pos, target, GameObjectOrder::AttackUnit);
		}
	}

	if (CanMove())
	{
		/*if (player->GetDistanceSqr(pos) < holdRadius ^ 2)
		{
			player->IssueOrder(&player->Pos, nullptr, GameObjectOrder::Stop);
			lastOrderCommand = GetTickCount() - 70;
			return;
		}
		int angle = round(Orbwalker::Randomizer(15, 120));
		if (GetTickCount() - lastOrderCommand < 70 + std::min(60, Game_Ping) && angle < 60)
		{
			return;
		}
		if (angle >= 60 && GetTickCount() - lastOrderCommand < 60)
		{
			return;
		}*/
		lastOrderCommand = GetTickCount();
		player->IssueOrder(&pos, nullptr, GameObjectOrder::MoveTo);
	}
}

float Orbwalker::Randomizer(int min, int max)
{
	return rand() % (max - min) + min;
}

bool Orbwalker::CanMove()
{
	if (_missileLaunched)
		return true;
	int localExtraWindup = 0;
	/*if (_championName == "Rengar" && (player.HasBuff("rengarqbase") || player.HasBuff("rengarqemp")))
	{
		localExtraWindup = 200;
	}*/
	return (GetTickCount() + Game_Ping / 2	>= LastAATick + player->GetAttackCastDelay() * 1000 + localExtraWindup); // EXTRA_Windup
}

bool Orbwalker::CanAttack()
{
	return GetTickCount() + Game_Ping / 2 + 25 >= LastAATick + player->GetAttackCastDelay() * 1000;
}

GameObject* Orbwalker::GetTarget()
{
	GameObject* result = nullptr;
	OrbwalkingMode mode = Mode();
	if (mode == OrbwalkingMode::LaneClear || mode == OrbwalkingMode::Mixed || mode == OrbwalkingMode::LastHit)
	{
		std::vector<GameObject*> enemyMinions = cpplinq::from(EntityManager::EntityMinions) >>
			cpplinq::where([](GameObject* g) {if (g->isValid()) return player->isInAARange(g) && g->Team != player->Team && !g->isDead; else return false; }) >>
			cpplinq::orderby_descending([](GameObject* g) {return g->CharName.operator std::string().find("Siege"); }) >>
			cpplinq::thenby([](GameObject* g) {return g->CharName.operator std::string().find("Super"); }) >>
			cpplinq::thenby([](GameObject* g) {return g->Health; }) >>
			cpplinq::thenby_descending([](GameObject* g) {return g->MaxHealth; }) >> cpplinq::to_vector();
		for (GameObject* minion : enemyMinions)
		{
			int t = (int)(player->GetAttackCastDelay() * 1000) - 100 + Game_Ping/2 + 1000 * (int)std::max(0, (int)(minion->GetDistance(player) - player->BoundingRadius())) / (int)player->GetBasicAttack()->SpellInfo->missleSpeed;
			float HealthPred = GetHealthPred(minion, t, 120); // FARM_DELAY
			float damage = player->GetTotalDamage();
			if (HealthPred <= damage)
				return minion;
		}
	}
	if (mode == OrbwalkingMode::LaneClear)
	{
		for (GameObject* turret : cpplinq::from(EntityManager::EntityTurrets) >> cpplinq::where([](GameObject* g) {if(g->isValid()) return !g->isDead && player->isInAARange(g) && g->Team != player->Team; else return false;}) >> cpplinq::to_vector())
		{
			return turret;
		}
		//toDo: Inhib + nexus
	}

	if (mode == OrbwalkingMode::Combo)
	{
		for (GameObject* champion : cpplinq::from(EntityManager::EntityChampions) >> cpplinq::where([](GameObject* g) {if(g->isValid()) return !g->isDead && player->isInAARange(g) && g->Team != player->Team; else return false; }) >> cpplinq::orderby_descending([](GameObject* g) {return g->GetDistance(Cursor::GetInstance()->cursorPos);}) >> cpplinq::to_vector())
		{
			return champion;
		}
	}

	if (mode == OrbwalkingMode::LaneClear || mode == OrbwalkingMode::Mixed)
	{
		GameObject* jMinion = cpplinq::from(EntityManager::EntityMinions) >>
			cpplinq::where([](GameObject* g) {if (g->isValid()) return player->isInAARange(g) && g->Team == 300 && !g->isDead; else return false; }) >>
			cpplinq::orderby_ascending([](GameObject* g) {return g->MaxHealth; }) >> cpplinq::min();
		if (jMinion != nullptr)
			return jMinion;
	}
	if (mode == OrbwalkingMode::LaneClear)
	{

	}
	return result;
}

float Orbwalker::GetHealthPred(GameObject* minion, int time, int delay = 70)
{
	float predictedDamage = 0;
	for (auto attack : ActiveAttacks)
	{
		float attackDamage = 0;
		if (!attack.second->Processed && attack.second->Source->isValid() && attack.second->Target->isValid() && attack.first == minion)
		{
			auto landTime = attack.second->initateTime + attack.second->delay + 1000 * std::max(0.f, minion->GetDistance(attack.second->Source) - attack.second->Source->BoundingRadius()) / attack.second->projectileSpeed + delay;
			if (landTime < GetTickCount() + time)
				attackDamage = attack.second->damage;
		}
		predictedDamage += attackDamage;
	}
	return minion->Health-predictedDamage;
}

void Orbwalker::ProcessSpell(GameObject* sender, CastData* castData)
{
	if (sender->Handle == player->Handle)
	{
		if (castData->isBasicAttack)
		{
			LastAATick = GetTickCount() - Game_Ping / 2;
			_missileLaunched = false;
			lastOrderCommand = 0;
		}
	}

	GameObject* target = ObjectManager::GetObjectArray()[castData->TargetHandle];
	if (sender->GetDistance(player) > 3000 || sender->Team != player->Team || sender->Type->Name.operator std::string() == std::string("AIHeroClient")
		|| !castData->isBasicAttack || !(target->Type->Name.operator std::string() == std::string("AIHeroClient") || sender->GetRange() < 135))
	{
		return;
	}
	
	ActiveAttacks.erase(sender);
	PredictedDamage* attackData = new PredictedDamage(sender, target, GetTickCount() - Game_Ping / 2, sender->GetAttackCastDelay() * 1000, sender->GetAttackDelay() * 1000, sender->GetBasicAttack()->SpellInfo->missleSpeed, sender->GetTotalDamage());
	ActiveAttacks.insert(std::pair<GameObject*, PredictedDamage*>(sender, attackData));
}