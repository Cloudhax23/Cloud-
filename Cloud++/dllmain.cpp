// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "Cloud.h"
#include "EventHandler.h"
#include "Orbwalker.h"
#include <d3dx9.h>
#pragma once

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Cloud::main();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

/*#define FN_NEWPATH 0x901B12
DWORD addy = 0x901B18;

void asmHookCallback(DWORD* EDI, GameObject* ESI, DWORD* EBP, DWORD* ESP, DWORD* EBX, DWORD* EDX, DWORD* ECX, DWORD* EAX) {
	std::cout << std::endl << "This:" << EDI << " GameObject: " << ESI->Name << " ARG2: " << EBP << " ARG3: " << ESP << " ARG4: " << EBX << " Path: " << EDX << " ARG6: " << ECX << " ARG7: " << EAX;
}

__declspec(naked) void asmhkNewPath()
{
	__asm
	{
		PUSHFD
		PUSHAD

		CALL asmHookCallback

		POPAD
		POPFD

		movaps xmm2, xmm6
		movaps xmm0, xmm7

		JMP[addy]
	};

}

void printMessage(char* fmt)
{
	std::cout << std::endl << fmt;
}*/

int Cloud::main()
{
	FreeConsole();
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	if (FindWindow(nullptr, L"League of Legends (TM) Client"))
	{
		Cloud::HookAll();
		GameObject* gameObject = ObjectManager::GetPlayer();
		Cloud::PrintObjectVariables(gameObject);
		Orbwalker::Orbwalker();
		std::cout << "BasicAttack: " << gameObject->GetBasicAttack() << " ProjectileSpeed: " << gameObject->GetBasicAttack()->SpellInfo->missleSpeed;
		//gameObject->HasBuff(std::string("testBuff"));
	}
	return 0;
};

int Cloud::detach()
{
	std::cout << std::endl << "Unhooking";
	//createObj->UnHook();
	return 0;
}