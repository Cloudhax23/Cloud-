#include "stdafx.h"
#include "Cloud.h"
#include "EntityManager.h"
#include "PolyHook.hpp"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_internal.h"
#include "Drawing.h"
#include "EventHandler.h"
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

bool Cloud::renderer = false;
typedef void(OnProcessSpell)(GameObject* sender, CastData* castData);
typedef void(OnRenderHUD)(int);
typedef void(OnKeyDown)(WPARAM);
typedef void(OnKeyUp)(WPARAM);
typedef void(OnBasicAttack)(GameObject* sender);
typedef void(OnCreateObject)(GameObject* object);
typedef void(OnDeleteObject)(GameObject* object);
typedef void(OnNewPath)(GameObject* object, D3DXVECTOR3* path, DWORD size);

PLH::VEHHook* UpdateGameHook;
typedef int(__thiscall* fnUpdateGame)(DWORD*);
fnUpdateGame offUpdateGame = 0;

PLH::VEHHook* CreateObjHook;
typedef int(__thiscall* fnCreateObj)(GameObject*, DWORD);
fnCreateObj offCreateObj = 0;

PLH::VEHHook* DeleteObjHook;
typedef int(__thiscall* fnDeleteObj)(DWORD*, GameObject*);
fnDeleteObj offDeleteObj = 0;

PLH::VEHHook* AddRemoveBuffHook;
typedef int(__thiscall* fnAddRemoveBuff)(DWORD*);
fnAddRemoveBuff offAddRemoveBuff = 0;

PLH::VEHHook* FinishCastHook;
typedef int(__thiscall* fnFinishCast)(DWORD*, DWORD*, DWORD*, DWORD*, DWORD*, DWORD*);
fnFinishCast offFinishCast = 0;

PLH::VEHHook* NewPathHook;
typedef int(__thiscall* fnNewPath)(DWORD*, DWORD*, GameObject*, DWORD*, float, DWORD*);
fnNewPath offNewPath = 0;

PLH::VEHHook* ProcessSpellHook;
typedef int(__thiscall* fnProcessSpell)(DWORD*, CastData*);
fnProcessSpell offProcessSpell;

PLH::VEHHook* RenderHUDHook;
typedef void(__thiscall* fnRenderHUD)(DWORD*);
fnRenderHUD offRenderHUD = 0;

PLH::VTableSwap* BeginSceneHook;
typedef HRESULT(WINAPI* fnBeginScene)(LPDIRECT3DDEVICE9 pDevice);
fnBeginScene offBeginScene = 0;

PLH::VTableSwap* EndSceneHook;
typedef HRESULT(WINAPI* fnEndScene)(LPDIRECT3DDEVICE9 pDevice);
fnEndScene offEndScene = 0;

PLH::VTableSwap* PresentSceneHook;
typedef HRESULT(WINAPI* fnPresentScene)(LPDIRECT3DDEVICE9 pDevice, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDR);
fnPresentScene offPresentScene = 0;

PLH::VTableSwap* ResetSceneHook;
typedef HRESULT(WINAPI* fnResetScene)(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* d3dPresentParams);
fnResetScene offResetScene = 0;

WNDPROC WndProcHook;
LRESULT WINAPI hkWndProc(HWND, UINT, WPARAM, LPARAM);

int __fastcall hkUpdateGame(DWORD* unk)
{
	auto ProtectionObject = UpdateGameHook->GetProtectionObject();
	std::for_each(EntityManager::EntityChampions.begin(), EntityManager::EntityChampions.end(), [](GameObject* gameObj)
	{
	});
	return offUpdateGame(unk);
}

int __fastcall hkCreateObj(GameObject* object, int edx, DWORD unk)
{
	auto ProtectionObject = CreateObjHook->GetProtectionObject();
	if (object->Team != 0)
	{
		EventHandler::TriggerEventHandler<OnCreateObject, GameObject*>("OnCreateObject", object);
		if (object->Type->Name.operator std::string() == std::string("AIMinionClient"))
		{
			EntityManager::EntityMinions.push_back(object);
		}
		if (object->Type->Name.operator std::string() == std::string("AITurret"))
		{
			EntityManager::EntityTurrets.push_back(object);
		}
		if (object->Type->Name.operator std::string() == std::string("AIHeroClient"))
		{
			EntityManager::EntityChampions.push_back(object);
		}
	}

	return offCreateObj(object, unk);
}

int __fastcall hkDeleteObj(DWORD* This, int edx, GameObject* object)
{
	auto ProtectionObject = DeleteObjHook->GetProtectionObject();
	EventHandler::TriggerEventHandler<OnDeleteObject, GameObject*>("OnDeleteObject", object);
	EntityManager::EntityMinions.remove(object);
	EntityManager::EntityTurrets.remove(object);
	EntityManager::EntityChampions.remove(object);
	EventHandler::TriggerEventHandler<OnDeleteObject, GameObject*>("OnDeleteObject", object);

	return offDeleteObj(This, object);
}

int __fastcall hkAddRemoveBuff(DWORD* This, int edx)
{
	auto ProtectionObject = AddRemoveBuffHook->GetProtectionObject();
	return offAddRemoveBuff(This);
}

int __fastcall hkFinishCast(DWORD* This, int edx, DWORD* a2, DWORD* a3, DWORD* a4, DWORD* a5, DWORD* a6)
{
	auto ProtectionObject = FinishCastHook->GetProtectionObject();
	//std::cout << std::endl << "This: " << This << " A2: " << a2 << " A3: " << a3 << " A4: " << a4 << " A5: " << a5 << " A6: " << a6;
	return offFinishCast(This, a2, a3, a4, a5, a6);
}

int __fastcall hkNewPath(DWORD* a1, int edx, DWORD* a2, GameObject* a3, DWORD* a4, float a5, DWORD* a6)
{
	auto ProtectionObject = NewPathHook->GetProtectionObject();
	DWORD* start = *(DWORD**)a2;
	DWORD* end = *(DWORD**)((DWORD)a2 + (DWORD)0x4);
	D3DXVECTOR3* ObjectPathing = *(D3DXVECTOR3**)((DWORD)a2);
	EventHandler::TriggerEventHandler<OnNewPath, GameObject*, D3DXVECTOR3*, DWORD>("OnNewPath", a3, ObjectPathing, (DWORD)((DWORD)end - (DWORD)start) / 12);
	return offNewPath(a1, a2, a3, a4, a5, a6);
}


int __fastcall hkProcessSpell(DWORD* SpellBook, int edx, CastData* SpellCastInfo)
{
	auto ProtectionObject = ProcessSpellHook->GetProtectionObject();
	EventHandler::TriggerEventHandler<OnProcessSpell, GameObject*, CastData*>("OnProcessSpell", ObjectManager::GetObjectArray()[SpellCastInfo->SenderHandle], SpellCastInfo);

	return offProcessSpell(SpellBook, SpellCastInfo);
}

HRESULT WINAPI hkBeginScene(LPDIRECT3DDEVICE9 pDevice)
{
	return offBeginScene(pDevice);
}

HRESULT WINAPI hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	if (!Cloud::renderer)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 1.0f / 60.0f;
		D3DDEVICE_CREATION_PARAMETERS d3dcp{ 0 };
		pDevice->GetCreationParameters(&d3dcp);
		io.Fonts->AddFontDefault();
		ImGui_ImplDX9_Init(d3dcp.hFocusWindow, pDevice);
		WndProcHook = (WNDPROC)SetWindowLongPtr(d3dcp.hFocusWindow, GWLP_WNDPROC, (LONG_PTR)hkWndProc);
		Drawing::InitalizePrimatives();
		Cloud::renderer = true;
	}

	Drawing::DrawFontText(Render::GetInstance()->ScreenSizeX/ 2, 0, D3DCOLOR_XRGB(255, 0, 0), "Cloud++", 10);
	ImGui_ImplDX9_NewFrame();
	Drawing::RenderHackMenu();
	ImGui::Render();
	return offEndScene(pDevice);
}

HRESULT WINAPI hkPresentScene(LPDIRECT3DDEVICE9 pDevice, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDR)
{
	return offPresentScene(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDR);
}

HRESULT WINAPI hkResetScene(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* d3dPresentParams)
{
	if (Drawing::DX_pFont)
	{
		Drawing::DX_pFont->OnResetDevice();
		Drawing::DX_pFont = nullptr;
	}
	if (Drawing::DX_pLine)
	{
		Drawing::DX_pLine->OnResetDevice();
		Drawing::DX_pLine = nullptr;
	}
	return offResetScene(pDevice, d3dPresentParams);
}
std::list<D3DXVECTOR3> Cloud::WayPoints;
void __fastcall hkRenderHUD(DWORD* This, int edx)
{
	auto ProtectionObject = RenderHUDHook->GetProtectionObject();
	if (Cloud::renderer)
	{
		GameObject* gameObject = ObjectManager::GetPlayer();
		Drawing::DrawCircle(gameObject->Pos, gameObject->GetRange(), D3DCOLOR_RGBA(60, 60, 245, 255), 5, false);
		Drawing::DrawCircle(Cursor::GetInstance()->cursorPos, 125, D3DCOLOR_RGBA(60, 60, 245, 255), 5, false);
		/*std::for_each(Cloud::WayPoints.begin(), Cloud::WayPoints.end(), [](D3DXVECTOR3 wayPoint)
		{
			Drawing::DrawCircle(wayPoint, 125 + 80, D3DCOLOR_RGBA(60, 60, 245, 255), 5, false);
		});*/
		//Cloud::IssueOrder(&Cursor::GetInstance()->cursorPos, nullptr, GameObjectOrder::MoveTo);
		//std::cout << std::endl << Cloud::GetBasicAttack();


		EventHandler::TriggerEventHandler<OnRenderHUD, int>("OnRenderHUD", 1);

	}
	return offRenderHUD(This);
}


LRESULT CALLBACK hkWndProc(HWND hwnd, UINT unt, WPARAM wprm, LPARAM lprm)
{
	ImGui_ImplDX9_WndProcHandler(hwnd, unt, wprm, lprm);
	if(unt == WM_KEYDOWN)
		EventHandler::TriggerEventHandler<OnKeyDown, WPARAM>("OnKeyDown", wprm);
	if (unt == WM_KEYUP)
		EventHandler::TriggerEventHandler<OnKeyDown, WPARAM>("OnKeyUp", wprm);
	return CallWindowProc(WndProcHook, hwnd, unt, wprm, lprm);
}


float Cloud::GetGameTime()
{
	return *(float*)(BASE + oGameTime);
}

void Cloud::CreateObject(GameObject* object)
{	

};

void Cloud::ProcessSpell(GameObject* sender, CastData* castData)
{
	//if (sender->CharName.operator std::string() == std::string("Twitch"))
	//{
	//	std::cout << std::endl << sender->CharName;
	//}
};

void Cloud::NewPath(GameObject* sender, D3DXVECTOR3* path, DWORD size)
{
	/*if (sender->CharName.operator std::string() == std::string("Nasus"))
	{
		WayPoints.clear();
		for(DWORD i = 0; i < size; i++)
		WayPoints.push_back(path[i]);
	}*/
};

int Cloud::HookAll()
{
	std::cout << "[Cloud++] Hooks Initiating" << std::endl;
	UpdateGameHook = new PLH::VEHHook();
	CreateObjHook = new PLH::VEHHook();
	DeleteObjHook = new PLH::VEHHook();
	AddRemoveBuffHook = new PLH::VEHHook();
	FinishCastHook = new PLH::VEHHook();
	NewPathHook = new PLH::VEHHook();
	ProcessSpellHook = new PLH::VEHHook();
	RenderHUDHook = new PLH::VEHHook();
	BeginSceneHook = new PLH::VTableSwap();
	EndSceneHook = new PLH::VTableSwap();
	PresentSceneHook = new PLH::VTableSwap();
	ResetSceneHook = new PLH::VTableSwap();
	Render* Renderer = Render::GetInstance();
	EventHandler::AddEventHandler("OnCreateObject", CreateObject);
	EventHandler::AddEventHandler("OnProcessSpell", ProcessSpell);
	EventHandler::AddEventHandler("OnNewPath", NewPath);
	/*

	FinishCastHook->SetupHook((BYTE*)(lol + FN_FINISHCAST), (BYTE*)&hkFinishCast, PLH::VEHHook::VEHMethod::INT3_BP);
	FinishCastHook->Hook();
	ofFinishCast = FinishCastHook->GetOriginal<fnFinishCast>();
	UpdateGameHook->SetupHook((BYTE*)(BASE + FN_ONGAMEUPDATE), (BYTE*)&hkUpdateGame, PLH::VEHHook::VEHMethod::INT3_BP);
	UpdateGameHook->Hook();
	offUpdateGame = UpdateGameHook->GetOriginal<fnUpdateGame>();
	*/
	AddRemoveBuffHook->SetupHook((BYTE*)(BASE + FN_ADDREMOVEBUFF), (BYTE*)&hkAddRemoveBuff, PLH::VEHHook::VEHMethod::INT3_BP);
	AddRemoveBuffHook->Hook();
	offAddRemoveBuff = AddRemoveBuffHook->GetOriginal<fnAddRemoveBuff>();

	CreateObjHook->SetupHook((BYTE*)(BASE + FN_CREATEOBJ), (BYTE*)&hkCreateObj, PLH::VEHHook::VEHMethod::INT3_BP);
	CreateObjHook->Hook();
	offCreateObj = CreateObjHook->GetOriginal<fnCreateObj>();
	
	DeleteObjHook->SetupHook((BYTE*)(BASE + FN_DELETEOBJ), (BYTE*)&hkDeleteObj, PLH::VEHHook::VEHMethod::INT3_BP);
	DeleteObjHook->Hook();
	offDeleteObj = DeleteObjHook->GetOriginal<fnDeleteObj>();
	
	/*NewPathHook->SetupHook((BYTE*)(BASE + FN_NEWPATH), (BYTE*)&hkNewPath, PLH::VEHHook::VEHMethod::INT3_BP);
	NewPathHook->Hook();
	offNewPath = NewPathHook->GetOriginal<fnNewPath>();
	*/
	ProcessSpellHook->SetupHook((BYTE*)(BASE + FN_PROCESSSPELL), (BYTE*)&hkProcessSpell, PLH::VEHHook::VEHMethod::INT3_BP);
	ProcessSpellHook->Hook();
	offProcessSpell = ProcessSpellHook->GetOriginal<fnProcessSpell>();
	
	RenderHUDHook->SetupHook((BYTE*)(BASE + FN_RENDERHUD), (BYTE*)&hkRenderHUD, PLH::VEHHook::VEHMethod::INT3_BP);
	RenderHUDHook->Hook();
	offRenderHUD = RenderHUDHook->GetOriginal<fnRenderHUD>();
	
	BeginSceneHook->SetupHook((BYTE*)Renderer->DeviceHandle->Device, 41, (BYTE*)&hkBeginScene);
	BeginSceneHook->Hook();
	offBeginScene = BeginSceneHook->GetOriginal<fnBeginScene>();

	EndSceneHook->SetupHook((BYTE*)Renderer->DeviceHandle->Device, 42, (BYTE*)&hkEndScene);
	EndSceneHook->Hook();
	offEndScene = EndSceneHook->GetOriginal<fnEndScene>();

	PresentSceneHook->SetupHook((BYTE*)Renderer->DeviceHandle->Device, 17, (BYTE*)&hkPresentScene);
	PresentSceneHook->Hook();
	offPresentScene = PresentSceneHook->GetOriginal<fnPresentScene>();

	ResetSceneHook->SetupHook((BYTE*)Renderer->DeviceHandle->Device, 16, (BYTE*)&hkResetScene);
	ResetSceneHook->Hook();
	offResetScene = ResetSceneHook->GetOriginal<fnResetScene>();

	std::cout << "[Cloud++] Hooks Completed!" << std::endl;

	EntityManager::Cache();
	return 1;
}

int Cloud::UnHookAll()
{
	CreateObjHook->UnHook();
	DeleteObjHook->UnHook();
	AddRemoveBuffHook->UnHook();
	FinishCastHook->UnHook();
	NewPathHook->UnHook();
	RenderHUDHook->UnHook();
	BeginSceneHook->UnHook();
	EndSceneHook->UnHook();
	PresentSceneHook->UnHook();
	ResetSceneHook->UnHook();
	return 1;
}

bool Cloud::isValidAddress(void* p)
{
	return PLH::IsValidPtr(p);
}

void Cloud::PrintObjectVariables(GameObject* gameObject)
{
	std::cout << std::endl << "Type: " << gameObject->Type->Name << std::endl << "Team: " << gameObject->Team << std::endl << "Name: " << gameObject->Name << std::endl
		<< "Pos: " << gameObject->Pos.x << " " << gameObject->Pos.y << " " << gameObject->Pos.z << std::endl << "LifeState: " << gameObject->LifeState << std::endl << "NetID: " << gameObject->NetworkID << std::endl << "Health/Max Health: " << gameObject->Health << "/" << gameObject->MaxHealth << std::endl << "BuffManager Length: " << gameObject->BuffManager.size() << std::endl << "Range: " << gameObject->GetRange() << std::endl;
};