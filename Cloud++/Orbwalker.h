#pragma once
#include <map>

enum class OrbwalkingMode
{
	None = 0,
	Combo = VK_SPACE,
	LaneClear = 0x56,
	LastHit = 0x58,
	Mixed = 0x43
};

class PredictedDamage
{
public:
	GameObject* Source;
	GameObject* Target;
	int initateTime;
	float delay;
	float animationTime;
	int projectileSpeed;
	float damage;
	bool Processed;
	PredictedDamage(GameObject* source, GameObject* target, int Time, float Delay, float AnimationTime, int ProjectileSpeed, float Damage)
	{
		Source = source;
		Target = target;
		initateTime = Time;
		delay = Delay;
		animationTime = AnimationTime;
		projectileSpeed = ProjectileSpeed;
		damage = Damage;
	}
};


class Orbwalker
{
public:
	Orbwalker();
	~Orbwalker();
	static GameObject* player;
	static void ProcessSpell(GameObject*, CastData*);
	static void CreateObj(GameObject*);
	static void DeleteObj(GameObject*);
	static void OnTick(int);
	static void Orbwalk(GameObject*, D3DXVECTOR3, int, int);
	static std::map<GameObject*, PredictedDamage*> ActiveAttacks;
	static float GetHealthPred(GameObject*, int, int);
	static OrbwalkingMode Mode();
	static int lastAttackCommand;
	static bool CanAttack();
	static bool CanMove();
	static GameObject* GetTarget();
	static float Randomizer(int, int);
	static int lastOrderCommand;
	static int LastAATick;
	static bool _missileLaunched;
};

