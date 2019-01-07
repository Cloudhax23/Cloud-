// Cloud++.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "String.h"
#include <iostream>
#include <d3dx9.h>
#include <string>
#include <vector>
#pragma once

#define STR_MERGE_IMPL(x, y)				x##y
#define STR_MERGE(x,y)						STR_MERGE_IMPL(x,y)
#define MAKE_PAD(size)						BYTE STR_MERGE(pad_, __COUNTER__) [ size ]

#define DEFINE_MEMBER_0(x, y)				x // y is for making it more readable
#define DEFINE_MEMBER_N(x,offset)			struct { MAKE_PAD(offset); x; }

#define FN_CREATEOBJ 0x48E660
#define FN_DELETEOBJ 0x418240
#define FN_REMOVEBUFF 0x7E7580
#define FN_FINISHCAST 0x313B00
#define FN_NEWPATH 0x71FBE0
#define FN_RENDERHUD 0x672780

#define oLocalPlayer 0x16CA794
#define oObjectManager 0x33480F0
#define oGameState 0x16ba3a8
#define oGameTime 0x336EE24
#define oRenderer 0x16CA064

struct DeviceHandler
{
	union
	{
		DEFINE_MEMBER_0(DWORD Trash, 0x0);
		DEFINE_MEMBER_N(LPDIRECT3DDEVICE9 Device, 0x14);
	};
};

struct Render
{
	union
	{
		DEFINE_MEMBER_0(DWORD Trash, 0x0);
		DEFINE_MEMBER_N(DeviceHandler* DeviceHandle, 0x10);
		DEFINE_MEMBER_N(int ScreenSizeX, 0x20);
		DEFINE_MEMBER_N(int ScreenSizeY, 0x24);
		DEFINE_MEMBER_N(D3DMATRIX viewMatrix, 0x90);
		DEFINE_MEMBER_N(D3DMATRIX projectionMatrix, 0xD0);
	};
	static Render* GetInstance()
	{
		return *(Render**)((DWORD)GetModuleHandle(NULL) + oRenderer);
	}
};

struct GameObjectType
{
	union {
		DEFINE_MEMBER_0(DWORD Int, 0x0);
		DEFINE_MEMBER_N(String Name, 0x4);
	};
};

struct BuffInstance
{
	union {
		DEFINE_MEMBER_0(int trash, 0x0);
		DEFINE_MEMBER_N(char Name[32], 0x4);
	};
};

struct Buff
{
	union {
		DEFINE_MEMBER_0(int Type, 0x0);
		DEFINE_MEMBER_N(BuffInstance* BuffInst, 0x8);
		DEFINE_MEMBER_N(float startTime, 0xC);
		DEFINE_MEMBER_N(float endTime, 0x10);
		DEFINE_MEMBER_N(DWORD baseBuffCount, 0x1C);
		DEFINE_MEMBER_N(DWORD modBuffCount, 0x20);
		DEFINE_MEMBER_N(float Stacks, 0x2C);
	};
	/*int count() const
	{
		return (modBuffCount - baseBuffCount) >> 3;
	}*/
};

struct Spell
{
	union {
		DEFINE_MEMBER_0(int whoKnows, 0x0);
		DEFINE_MEMBER_N(float CDEndTime, 0x8);
	};
};

struct GameObject
{
	union {
		DEFINE_MEMBER_0(DWORD VTable, 0x0);
		DEFINE_MEMBER_N(GameObjectType* Type, 0x4);
		DEFINE_MEMBER_N(short Handle, 0x8);
		DEFINE_MEMBER_N(int Team, 0x14);
		DEFINE_MEMBER_N(String Name, 0x20);
		DEFINE_MEMBER_N(D3DXVECTOR3 Pos, 0x50);
		DEFINE_MEMBER_N(DWORD LifeState, 0xA4);
		DEFINE_MEMBER_N(DWORD NetworkID, 0xF0);
		DEFINE_MEMBER_N(float Mana, 0x2D8);
		DEFINE_MEMBER_N(float MaxMana, 0x2E8);
		DEFINE_MEMBER_N(float Health, 0x650);
		DEFINE_MEMBER_N(float MaxHealth, 0x660);
		DEFINE_MEMBER_N(std::vector<Buff*> BuffManager, 0x1554);
		DEFINE_MEMBER_N(Spell* SpellArray[64], 0x2398);
	};
};

template<typename T >
T ReadAddress(DWORD pBase)
{
	return *(T*)((DWORD)pBase);
}

template<typename T >
T ReadOffset(DWORD pBase, DWORD dwIndex)
{
	return *(T*)((DWORD)pBase + dwIndex);
}

/*DWORD getVFTableLOL(DWORD classInst, DWORD funcIndex)
{
	DWORD VFTable = ReadAddress<DWORD>(classInst);
	DWORD hookAddress = VFTable + funcIndex * sizeof(DWORD);
	return ReadAddress<DWORD>(hookAddress);
}

void MakeJMPLOL(BYTE *pAddress, DWORD dwJumpTo, DWORD dwLen)
{
	DWORD dwOldProtect, dwBkup, dwRelAddr;
	VirtualProtect(pAddress, dwLen, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	dwRelAddr = (DWORD)(dwJumpTo - (DWORD)pAddress) - 5;
	*pAddress = 0xE9;
	*((DWORD *)(pAddress + 0x1)) = dwRelAddr;
	for (DWORD x = 0x5; x < dwLen; x++) *(pAddress + x) = 0x90;
	VirtualProtect(pAddress, dwLen, dwOldProtect, &dwBkup);
	return;
}*/