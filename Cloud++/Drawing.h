#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

class Drawing
{
public:
	static ID3DXLine* DX_pLine;
	static ID3DXFont* DX_pFont;
	static IDirect3DDevice9* r3dDevice;
	static IDirect3DVertexBuffer9* VertexBuffer;
	static IDirect3DVertexDeclaration9* VertexDeclaration;
	static ID3DXEffect* Effect;
	static D3DXHANDLE Technique;
	static bool VertexesCreated;
	static void DrawRect(int x, int y, int w, int h, D3DCOLOR color);
	static void DrawFontText(float x, float y, D3DCOLOR color, char* text, int size);
	static void DrawCircle(D3DXVECTOR3 pos, float radius, D3DCOLOR color, int width, bool zdeep);
	//static void DrawLine(float x, float y, float x2, float y2, float thickness, D3DCOLOR LineCol);
	static void DrawLine(float x, float y, float x2, float y2, float thickness, D3DCOLOR color);
	static void CreateVertexes();
	static void InitalizePrimatives();
	static void RenderHackMenu();
};

class Menu
{
public:
	static bool EnableOrbwalker;
};