#include "stdafx.h"
#include "String.h"
#include <d3dx9.h>
#pragma once

#define STR_MERGE_IMPL(x, y)				x##y
#define STR_MERGE(x,y)						STR_MERGE_IMPL(x,y)
#define MAKE_PAD(size)						BYTE STR_MERGE(pad_, __COUNTER__) [ size ]

#define DEFINE_MEMBER_0(x, y)				x // y is for making it more readable
#define DEFINE_MEMBER_N(x,offset)			struct { MAKE_PAD(offset); x; }
#define IS_NULL_RETN(OBJECT, ADDR, RETN) if ((DWORD*)(OBJECT + ADDR) == nullptr) { return RETN; }

#pragma region OldPatches   
/*#define FN_CREATEOBJ 0x1E9FB0 7.18
#define FN_DELETEOBJ 0x18F040
#define FN_ADDREMOVEBUFF 0x6E9170
#define FN_FINISHCAST 0x313B00
#define FN_NEWPATH 0x671F20
#define FN_RENDERHUD 0x56B670
#define FN_ONGAMEUPDATE 0x230480
#define FN_PROCESSSPELL 0x4EC030

#define oLocalPlayer 0x16AF874
#define oObjectManager 0x332F1D8
#define oGameState 0x16ba3a8
#define oGameTime 0x3345040
#define oRenderer 0x16B11D0
*/
#pragma endregion Old Patches from League 

#define FN_CREATEOBJ 0x2E4520
#define FN_DELETEOBJ 0x2D8BB0
#define FN_ADDREMOVEBUFF 0x50F470
#define FN_NEWPATH 0xB2B7F0 // Update
#define FN_RENDERHUD 0x62C1F0
#define FN_ONGAMEUPDATE 0xFFFFFFFF //Who cares...
#define FN_PROCESSSPELL 0x541B50
#define FN_DOCASTSPELL 0x5417C0
#define FN_ATTACKDELAY 0x546A40
#define FN_ATTACKCASTDELAY 0x546AA0
#define FN_BASICATTACK 0x1C1490
#define oLocalPlayer 0x2E2FEF8

#define oObjectManager 0x2E2D658
#define oGameState 0x0
#define oGameTime 0x21D3E14
#define oRenderer 0x2E54250
#define oIssueOrder 0x1C9EA0
#define oHUD 0x1FA2E8
#define oLoLCursor 0x1585E18
#define DLLEXPORT __declspec( dllexport )
#define BASE (DWORD)GetModuleHandle(NULL)

template<typename Function >
Function GetVirtual(PVOID pBase, DWORD dwIndex)
{
	PDWORD* VTablePointer = (PDWORD*)pBase;
	PDWORD VTableFunctionBase = *VTablePointer;
	DWORD dwAddress = VTableFunctionBase[dwIndex];

	return (Function)(dwAddress);
}

enum class GameObjectOrder
{
	HoldPosition = 1,
	MoveTo,
	AttackUnit,
	AutoAttackPet,
	AutoAttack,
	MovePet,
	AttackTo,
	Stop = 10
};

struct LeagueHUD
{
	union
	{

	};
	static LeagueHUD* GetInstance()
	{
		return *(LeagueHUD**)(BASE + oHUD);
	}
	static DWORD GetPing()
	{
		return (DWORD)(((DWORD*)(GetInstance() + 0x1104)) + 0x8C);
	}
};

struct Cursor
{
	union
	{
		DEFINE_MEMBER_0(DWORD unk, 0x0);
		DEFINE_MEMBER_N(D3DXVECTOR3 cursorPos, 0x90);
	};
	static Cursor* GetInstance()
	{
		return *(Cursor**)(BASE + oLoLCursor);
	}
};

struct DeviceHandler
{
	union
	{
		DEFINE_MEMBER_0(DWORD unk, 0x0);
		DEFINE_MEMBER_N(LPDIRECT3DDEVICE9 Device, 0x14);
	};
};

struct Render
{
	union
	{
		DEFINE_MEMBER_0(DWORD unk, 0x0);
		DEFINE_MEMBER_N(DeviceHandler* DeviceHandle, 0x10);
		DEFINE_MEMBER_N(int ScreenSizeX, 0x20);
		DEFINE_MEMBER_N(int ScreenSizeY, 0x24);
		DEFINE_MEMBER_N(D3DMATRIX viewMatrix, 0x8C);
		DEFINE_MEMBER_N(D3DMATRIX projectionMatrix, 0xCC);
	};
	static Render* GetInstance()
	{
		return *(Render**)(BASE + oRenderer);
	}
};

struct GameObjectType
{
	union 
	{
		DEFINE_MEMBER_0(DWORD Int, 0x0);
		DEFINE_MEMBER_N(String Name, 0x4);
	};
};

struct BuffInstance
{
	union 
	{
		DEFINE_MEMBER_0(int unk, 0x0);
		DEFINE_MEMBER_N(char Name[32], 0x8);
	};
	bool BuffInstance::isValid()
	{
		IS_NULL_RETN(this, 0x18, false);
		IS_NULL_RETN(this, 0x1C, false);
		IS_NULL_RETN(this, 0x4, false);
		IS_NULL_RETN(this, 0x68, false);
		IS_NULL_RETN(this, static_cast<int>(0x8), false);

		return this != nullptr && *reinterpret_cast<DWORD*>(this + 0x18) != *reinterpret_cast<DWORD*>(this + 0x1C) && *reinterpret_cast<DWORD*>(this + 0x4) || *reinterpret_cast<BYTE*>(this + 0x68);
	}
};

struct Buff
{
	union 
	{
		DEFINE_MEMBER_0(int Type, 0x0);
		DEFINE_MEMBER_N(BuffInstance* BuffInst, 0x4);
		DEFINE_MEMBER_N(float startTime, 0xC);
		DEFINE_MEMBER_N(float endTime, 0x10);
		DEFINE_MEMBER_N(DWORD baseBuffCount, 0x20);
		DEFINE_MEMBER_N(DWORD modBuffCount, 0x24);
		DEFINE_MEMBER_N(DWORD Stacks, 0x6C);
	};
};

struct SpellData
{
	union
	{
		DEFINE_MEMBER_0(int unk, 0x0);
		DEFINE_MEMBER_N(DWORD Flags, 0x4);
		DEFINE_MEMBER_N(DWORD AffectsTypeFlags, 0x8);
		DEFINE_MEMBER_N(DWORD AffectsStatusFlags, 0xC);
		DEFINE_MEMBER_N(DWORD RequiredUnitTags, 0x10);
		DEFINE_MEMBER_N(DWORD ExcludedUnitTags, 0x28);
		DEFINE_MEMBER_N(DWORD PlatformSpellInfo, 0x58);
		DEFINE_MEMBER_N(String DisplayName, 0x58);
		DEFINE_MEMBER_N(String AlternateName, 0x7C);
		DEFINE_MEMBER_N(String Description, 0x94);
		DEFINE_MEMBER_N(String DynamicTooltip, 0xB8);
		DEFINE_MEMBER_N(String DynamicExtended, 0xC4);
		DEFINE_MEMBER_N(float Coefficient, 0x1F4);
		DEFINE_MEMBER_N(float Coefficient2, 0x1F8);
		DEFINE_MEMBER_N(String AnimationName, 0x1FC);
		DEFINE_MEMBER_N(String AnimationLoopName, 0x208);
		DEFINE_MEMBER_N(String AnimationWinddownName, 0x214);
		DEFINE_MEMBER_N(String AnimationLeadOutName, 0x220);
		DEFINE_MEMBER_N(String MinimapIconName, 0x238);
		DEFINE_MEMBER_N(float CastTime, 0x250);
		DEFINE_MEMBER_N(float ChannelDuration, 0x254);
		DEFINE_MEMBER_N(float CoolDownTime, 0x274);
		DEFINE_MEMBER_N(float DelayCastOffsetPercent, 0x290);
		DEFINE_MEMBER_N(float DelayTotalTimePercent, 0x294);
		DEFINE_MEMBER_N(float PreCastLockoutDeltaTime, 0x298);
		DEFINE_MEMBER_N(float PostCastLockoutDeltaTime, 0x29C);
		DEFINE_MEMBER_N(float StartCooldown, 0x29C);
		DEFINE_MEMBER_N(float CastRangeGrowthMax, 0x2A4);
		DEFINE_MEMBER_N(float CastRangeGrowthStartTime, 0x2C0);
		DEFINE_MEMBER_N(float CastRangeGrowthDuration, 0x2DC);
		DEFINE_MEMBER_N(float ChargeUpdateInterval, 0x2F8);
		DEFINE_MEMBER_N(float CancelChargeOnRecastTime, 0x2FC);
		DEFINE_MEMBER_N(bool UpdateToCasterPositionAtEndOfCastTime, 0x300);
		DEFINE_MEMBER_N(DWORD MaxAmmo, 0x304);
		DEFINE_MEMBER_N(DWORD AmmoUsed, 0x320);
		DEFINE_MEMBER_N(float AmmoRechargeTime, 0x33C);
		DEFINE_MEMBER_N(bool IsDelayedByCastLocked, 0x358);
		DEFINE_MEMBER_N(bool AmmoNotAffectedByCDR, 0x359);
		DEFINE_MEMBER_N(bool CooldownNotAffectedByCDR, 0x35A);
		DEFINE_MEMBER_N(bool AmmoCountHiddenInUI, 0x35B);
		DEFINE_MEMBER_N(bool CostAlwaysShownInUI, 0x35C);
		DEFINE_MEMBER_N(bool CannotBeSuppressed, 0x35D);
		DEFINE_MEMBER_N(bool CanCastWhileDisabled, 0x35E);
		DEFINE_MEMBER_N(bool CanTriggerChargeSpellWhileDisabled, 0x35F);
		DEFINE_MEMBER_N(bool CanCastOrQueueWhileCasting, 0x360);
		DEFINE_MEMBER_N(bool CanOnlyCastWhileDisabled, 0x361);
		DEFINE_MEMBER_N(bool CantCancelWhileWindingUp, 0x362);
		DEFINE_MEMBER_N(bool CantCancelWhileChanneling, 0x363);
		DEFINE_MEMBER_N(bool CantCastWhileRooted, 0x364);
		DEFINE_MEMBER_N(bool ChannelIsInterruptedByDisables, 0x365);
		DEFINE_MEMBER_N(bool ChannelIsInterruptedByAttacking, 0x366);
		DEFINE_MEMBER_N(bool ApplyAttackDamage, 0x367);
		DEFINE_MEMBER_N(bool ApplyAttackEffect, 0x368);
		DEFINE_MEMBER_N(bool ApplyMaterialOnHitSound, 0x369);
		DEFINE_MEMBER_N(bool DoesntBreakChannels, 0x36A);
		DEFINE_MEMBER_N(bool BelongsToAvatar, 0x36B);
		DEFINE_MEMBER_N(bool IsDisabledWhileDead, 0x36C);
		DEFINE_MEMBER_N(bool CanOnlyCastWhileDead, 0x36D);
		DEFINE_MEMBER_N(bool CursorChangesInGrass, 0x36E);
		DEFINE_MEMBER_N(bool CursorChangesInTerrain, 0x36F);
		DEFINE_MEMBER_N(bool ProjectTargetToCastRange, 0x370);
		DEFINE_MEMBER_N(bool SpellRevealsChampion, 0x371);
		DEFINE_MEMBER_N(bool UseMinimapTargeting, 0x372);
		DEFINE_MEMBER_N(bool CastRangeUseBoundingBoxes, 0x373);
		DEFINE_MEMBER_N(bool MinimapIconRotation, 0x374);
		DEFINE_MEMBER_N(bool UseChargeChanneling, 0x375);
		DEFINE_MEMBER_N(bool UseChargeTargeting, 0x376);
		DEFINE_MEMBER_N(bool CanMoveWhileChanneling, 0x377);
		DEFINE_MEMBER_N(bool DisableCastBar, 0x378);
		DEFINE_MEMBER_N(bool ShowChannelBar, 0x379);
		DEFINE_MEMBER_N(bool AlwaysSnapFacing, 0x37A);
		DEFINE_MEMBER_N(bool UseAnimatorFramerate, 0x37B);
		DEFINE_MEMBER_N(bool HaveHitEffect, 0x37C);
		DEFINE_MEMBER_N(bool HaveHitBone, 0x37D);
		DEFINE_MEMBER_N(bool IsToggleSpell, 0x37E);
		DEFINE_MEMBER_N(bool DoNotNeedToFaceTarget, 0x37F);
		DEFINE_MEMBER_N(bool NoWinddownIfCancelled, 0x380);
		DEFINE_MEMBER_N(bool IgnoreRangeCheck, 0x381);
		DEFINE_MEMBER_N(bool OrientRadiusTextureFromPlayer, 0x382);
		DEFINE_MEMBER_N(bool UseAutoattackCastTime, 0x383);
		DEFINE_MEMBER_N(bool IgnoreAnimContinueUntilCastFrame, 0x384);
		DEFINE_MEMBER_N(bool HideRangeIndicatorWhenCasting, 0x385);
		DEFINE_MEMBER_N(bool UpdateRotationWhenCasting, 0x386);
		DEFINE_MEMBER_N(bool PingableWhileDisabled, 0x387);
		DEFINE_MEMBER_N(bool ConsideredAsAutoAttack, 0x388);
		DEFINE_MEMBER_N(bool DoesNotConsumeMana, 0x389);
		DEFINE_MEMBER_N(bool DoesNotConsumeCooldown, 0x38A);
		DEFINE_MEMBER_N(bool LockedSpellOriginationCastID, 0x38B);
		DEFINE_MEMBER_N(short MinimapIconDisplayFlag, 0x38C);
		DEFINE_MEMBER_N(float CastRange, 0x390);
		DEFINE_MEMBER_N(float CastRangeDisplayOverride, 0x3AC);
		DEFINE_MEMBER_N(float CastRadius, 0x3C8);
		DEFINE_MEMBER_N(float CastRadiusSecondary, 0x3E4);
		DEFINE_MEMBER_N(float CastConeAngle, 0x400);
		DEFINE_MEMBER_N(float CastConeDistance, 0x404);
		DEFINE_MEMBER_N(float CastTargetAdditionalUnitsRadius, 0x408);
		DEFINE_MEMBER_N(float LuaOnMissileUpdateDistanceInterval, 0x40C);
		DEFINE_MEMBER_N(DWORD CastType, 0x414);
		DEFINE_MEMBER_N(float CastFrame, 0x418);
		DEFINE_MEMBER_N(float SpellDamageRatio, 0x41C);
		DEFINE_MEMBER_N(float PhysicalDamageRatio, 0x420);
		DEFINE_MEMBER_N(DWORD BuffTimerSimulationType, 0x424);
		DEFINE_MEMBER_N(bool HideCDOnPostSpellCast, 0x428);
		DEFINE_MEMBER_N(float missleSpeed, 0x42C);
		DEFINE_MEMBER_N(char* MissileEffectKey, 0x430);
		DEFINE_MEMBER_N(String MissileEffectName, 0x434);
		DEFINE_MEMBER_N(char* MissileEffectPlayerKey, 0x440);
		DEFINE_MEMBER_N(String MissileEffectPlayerName, 0x444);
		DEFINE_MEMBER_N(char* MissileEffectEnemyKey, 0x450);
		DEFINE_MEMBER_N(String MissileEffectEnemyName, 0x454);
		DEFINE_MEMBER_N(float LineWidth, 0x460);
		DEFINE_MEMBER_N(float LineDragLength, 0x464);
		DEFINE_MEMBER_N(DWORD LookAtPolicy, 0x468);
		DEFINE_MEMBER_N(DWORD HitEffectOrientType, 0x46C);
		DEFINE_MEMBER_N(String HitBoneName, 0x470);
		DEFINE_MEMBER_N(char* HitEffectKey, 0x47C);
		DEFINE_MEMBER_N(String HitEffectName, 0x480);
		DEFINE_MEMBER_N(char* HitEffectPlayerKey, 0x48C);
		DEFINE_MEMBER_N(String HitEffectPlayerName, 0x490);
		DEFINE_MEMBER_N(char* AfterEffectKey, 0x49C);
		DEFINE_MEMBER_N(String AfterEffectName, 0x4A0);
		DEFINE_MEMBER_N(DWORD DeathRecapPriority, 0x4AC);
		DEFINE_MEMBER_N(D3DXVECTOR3 ParticleStartOffset, 0x4B0);
		DEFINE_MEMBER_N(float FloatVarsDecimals, 0x4BC);
		DEFINE_MEMBER_N(float Mana, 0x4FC);
		DEFINE_MEMBER_N(bool ManaUiOverride, 0x514);
		DEFINE_MEMBER_N(DWORD SelectionPriority, 0x52C);
		DEFINE_MEMBER_N(String VOEventCategory, 0x530);
		DEFINE_MEMBER_N(DWORD AIData, 0x53C);
		DEFINE_MEMBER_N(float SpellCooldownOrSealedQueueThreshold, 0x554);
		DEFINE_MEMBER_N(DWORD ClientData, 0x558);
		DEFINE_MEMBER_N(DWORD TargetingType, 0x614);

	};
};

struct Spell
{
	union 
	{
		DEFINE_MEMBER_0(int unk, 0x0);
		DEFINE_MEMBER_N(SpellData* SpellInfo, 0x34); // ERROR: Client Tried to cast a spell fro
	};
};

struct CastData
{
	union
	{
		DEFINE_MEMBER_0(Spell* SpellInfo, 0x0);
		DEFINE_MEMBER_N(short SenderHandle, 0x20);
		DEFINE_MEMBER_N(D3DXVECTOR3 startPos, 0x38);
		DEFINE_MEMBER_N(D3DXVECTOR3 endPos, 0x44);
		DEFINE_MEMBER_N(short TargetHandle , 0x6C);
		DEFINE_MEMBER_N(bool isBasicAttack, 0x48D);

	};
};

struct MissileData
{
	union
	{
		DEFINE_MEMBER_0(String missileName, 0x0);
	};
};

struct Missile
{
	union
	{
		DEFINE_MEMBER_0(Spell* unk, 0x0);
		DEFINE_MEMBER_N(short SenderHandle, 0x20);
		DEFINE_MEMBER_N(bool isAutoAttack, 0x68);
		DEFINE_MEMBER_N(short TargetHandle, 0x6C);
	};
};

struct GameObject
{
	union
	{
		DEFINE_MEMBER_0(DWORD* VTable, 0x0);
		DEFINE_MEMBER_N(GameObjectType* Type, 0x4);
		DEFINE_MEMBER_N(short Handle, 0x8);
		DEFINE_MEMBER_N(int Team, 0x14);
		DEFINE_MEMBER_N(String Name, 0x1C);
		DEFINE_MEMBER_N(DWORD NetworkID, 0x6C);
		DEFINE_MEMBER_N(DWORD LifeState, 0x98);
		DEFINE_MEMBER_N(bool isDead, 0xF0);
		DEFINE_MEMBER_N(D3DXVECTOR3 Pos, 0xF4);
		DEFINE_MEMBER_N(Missile missile, 0x104);
		DEFINE_MEMBER_N(float Mana, 0x2C4);
		DEFINE_MEMBER_N(float MaxMana, 0x2D4);
		DEFINE_MEMBER_N(float Health, 0x670);
		DEFINE_MEMBER_N(float MaxHealth, 0x680);
		DEFINE_MEMBER_N(float BonusDamage, 0x984);
		DEFINE_MEMBER_N(float BaseDamage, 0xA08);
		DEFINE_MEMBER_N(float Range, 0xA4C);
		DEFINE_MEMBER_N(std::vector<Buff*> BuffManager, 0x1278);
		DEFINE_MEMBER_N(String CharName, 0x1764);
		DEFINE_MEMBER_N(DWORD* SpellDataInst, 0x1CA0);
		DEFINE_MEMBER_N(Spell* SpellArray[4], 0x21D0);
	};
	float GameObject::BoundingRadius()
	{
		return GetVirtual<float(__fastcall*)(GameObject*)>(this, 37)(this);
	}
	bool GameObject::HasBuff(std::string buffName)
	{
		auto Begin = *this->BuffManager.begin();
		auto End = *this->BuffManager.end();
		if (Begin && End)
		{
			for (UINT i = 0; i < (End - Begin) / sizeof(Buff); i++)
			{
				auto buffNode = Begin + i;
				auto buffInst = buffNode->BuffInst;
				if (buffNode && buffInst)
				{
					if (buffInst->isValid())
					{
						std::cout << std::endl << " Buffname: " << buffInst->Name;
						if (buffInst->Name == buffName.c_str())
							return true;
					}
				}
			}
		}
		return false;
	}
	bool GameObject::isValid()
	{
		return this != nullptr;
	}
	bool GameObject::isMe()
	{
		return false;
		//return this->Handle == ObjectManager::GetPlayer()->Handle;
	}
	bool GameObject::isInAARange(GameObject* target)
	{
		return this->GetDistance(target) <= this->GetRange();
	}
	float GameObject::GetDistance(GameObject* target)
	{
		return sqrt(pow((target->Pos.x - this->Pos.x), 2) + pow((target->Pos.y - this->Pos.y), 2) + pow((target->Pos.z - this->Pos.z), 2));
	}
	float GameObject::GetDistance(D3DXVECTOR3 target)
	{
		return sqrt(pow((target.x - this->Pos.x), 2) + pow((target.y - this->Pos.y), 2) + pow((target.z - this->Pos.z), 2));
	}
	float GameObject::GetDistanceSqr(D3DXVECTOR3 target)
	{
		return pow((target.x - this->Pos.x), 2) + pow((target.y - this->Pos.y), 2) + pow((target.z - this->Pos.z), 2);
	}
	float GameObject::GetDistanceSqr(GameObject* target)
	{
		return pow((target->Pos.x - this->Pos.x), 2) + pow((target->Pos.y - this->Pos.y), 2) + pow((target->Pos.z - this->Pos.z), 2);
	}
	Spell* GameObject::GetBasicAttack()
	{
		return *reinterpret_cast<Spell**(__thiscall*)(void*, int)>(BASE + FN_BASICATTACK)(this, 64);
	}
	float GameObject::GetAttackDelay()
	{
		__try
		{
			return reinterpret_cast<float(__cdecl*)(GameObject*, int)>(BASE + FN_ATTACKCASTDELAY)(this, 64);
		}
		__except (1) { return 0; }
	}
	float GameObject::GetAttackCastDelay()
	{
		__try
		{
			return reinterpret_cast<float(__cdecl*)(GameObject*, int)>(BASE + FN_ATTACKDELAY)(this, 1);
		}
		__except (1) { return 0; }
	}
	float GameObject::GetRange()
	{
		return this->Range + this->BoundingRadius();
	}
	float GameObject::GetTotalDamage()
	{
		return this->BaseDamage + this->BonusDamage;
	}
	bool GameObject::IssueOrder(D3DXVECTOR3* position, GameObject* unit, GameObjectOrder order)
	{
		if (position == nullptr)
		{
			return false;
		}

		if (order == GameObjectOrder::AttackUnit && unit == nullptr)
		{
			return false;
		}
		auto issueOrderFlags1 = 0x0000000;
		auto issueOrderFlags2 = 0x0000000;

		switch (order)
		{
		case GameObjectOrder::HoldPosition:
			reinterpret_cast<bool(__thiscall*)(GameObject*, GameObjectOrder, D3DXVECTOR3*, GameObject*, DWORD, DWORD, bool)>(BASE + oIssueOrder)(this, GameObjectOrder::Stop, position, nullptr, 0, 0, true);
			issueOrderFlags1 = 0x0000000;
			issueOrderFlags2 = 0x0000001;
			break;
		case GameObjectOrder::MoveTo:
		case GameObjectOrder::AttackTo:
		case GameObjectOrder::AttackUnit:
		case GameObjectOrder::AutoAttack:
		case GameObjectOrder::AutoAttackPet:
			issueOrderFlags1 = 0xffffff00;
			break;
		case GameObjectOrder::Stop:
			issueOrderFlags2 = 0x0000001;
			break;
		case GameObjectOrder::MovePet:
			break;
		}
		return reinterpret_cast<bool(__thiscall*)(GameObject*, GameObjectOrder, D3DXVECTOR3*, GameObject*, DWORD, DWORD, bool)>(BASE + oIssueOrder)(this, order, position, unit, issueOrderFlags1, issueOrderFlags2, true);
	};
};

struct ObjectManager
{
	union 
	{
		DEFINE_MEMBER_0(DWORD* ObjectArray[10000], 0x0);
		DEFINE_MEMBER_N(int maxObjects, 0x4);
		DEFINE_MEMBER_N(int objectsUsed, 0x8);
		DEFINE_MEMBER_N(int maxObjectIndex, 0xC);
		DEFINE_MEMBER_N(int maxPlayerIndex, 0x10);
		DEFINE_MEMBER_N(int totalObjectIndex, 0x18);
	};
	static GameObject* GetPlayer()
	{
		return *(GameObject**)(BASE + oLocalPlayer);
	}
	static ObjectManager* GetInstance()
	{
		return (ObjectManager*)(BASE + oObjectManager);
	}
	static GameObject** GetObjectArray()
	{
		return *(GameObject***)(((ObjectManager*)(BASE + oObjectManager))->ObjectArray);
	}
};

class Cloud
{
public:
	static int main();
	static int detach();
	static int HookAll();
	static int UnHookAll();
	static void CreateObject(GameObject* object);
	static void ProcessSpell(GameObject* sender, CastData* castData);
	static void NewPath(GameObject* object, D3DXVECTOR3* path, DWORD size);
	static std::list<D3DXVECTOR3> WayPoints;
	static float GetGameTime();
	static void PrintObjectVariables(GameObject* gameObject);
	static bool isValidAddress(void*  p);
	static bool renderer;
};