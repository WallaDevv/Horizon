#include "Hooks.h"
#include "../Menu.h"
#include "../imgui\imgui_impl_dx9.h"
#include "../imgui\imgui_impl_win32.h"
#include "../imgui\imgui_freetype.h"
#include <d3dx9tex.h>
#include "../features/AntiAims.h"
#include "../fonts/prime.h"
#include <chrono>

void GUI_Init(IDirect3DDevice9* pDevice)
{
	if (!vars.menu.guiinited)
	{
		ImGui::CreateContext();

		ImGui_ImplWin32_Init(csgo->Init.Window);
		ImGui_ImplDX9_Init(pDevice);

		ImGuiIO& io = ImGui::GetIO();

		ImFontConfig cfg;
		fonts::menu_main = io.Fonts->AddFontFromFileTTF("C:/windows/fonts/Russoone-Regular.ttf", 12, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		fonts::menu_desc = io.Fonts->AddFontFromFileTTF("C:/windows/fonts/Russoone-Regular.ttf", 13, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		fonts::menu_fix = io.Fonts->AddFontFromFileTTF("C:/windows/fonts/Russoone-Regular.ttf", 10.7, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		fonts::very_small = io.Fonts->AddFontFromFileTTF("C:/windows/fonts/Russoone-Regular.ttf", 9, &cfg, io.Fonts->GetGlyphRangesCyrillic());

		fonts::lby_indicator = io.Fonts->AddFontFromFileTTF("C:/windows/fonts/Russoone-Regular.ttf", 25, &cfg, io.Fonts->GetGlyphRangesCyrillic());

		ImGuiFreeType::BuildFontAtlas(io.Fonts, 0x00/*ImGuiFreeType::RasterizerFlags::MonoHinting | ImGuiFreeType::RasterizerFlags::Monochrome*/);


		vars.menu.guiinited = true;
	}
}

LPDIRECT3DTEXTURE9 img = nullptr;
LPD3DXSPRITE sprite = nullptr;



string UnixTimeToExpiryDate(long long unixTime) {
	string s, m, h;
	if (unixTime < 0) {
		csgo->DoUnload = true;
		return string();
	}

	s = std::to_string(unixTime % 60);
	unixTime /= 60;
	m = std::to_string(unixTime % 60);
	unixTime /= 60;
	h = std::to_string(unixTime % 24);
	unixTime /= 24;

	if (atoi(h.c_str()) > 1000)
		return "Never :)";

	return std::to_string(unixTime)
		+ "d:" + ("0" + h).substr(h.length() - 1)
		+ "h:" + ("0" + m).substr(m.length() - 1)
		+ "m:" + ("0" + s).substr(s.length() - 1)
		+ "s";
}
IDirect3DStateBlock9* pixel_state = NULL; IDirect3DVertexDeclaration9* vertDec; IDirect3DVertexShader9* vertShader;
DWORD dwOld_D3DRS_COLORWRITEENABLE;

void SaveState(IDirect3DDevice9* pDevice)
{
	pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
	pDevice->GetVertexDeclaration(&vertDec);
	pDevice->GetVertexShader(&vertShader);
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
	pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);
}

void RestoreState(IDirect3DDevice9* pDevice) 
{
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
	pDevice->SetVertexDeclaration(vertDec);
	pDevice->SetVertexShader(vertShader);
}
static HRESULT __stdcall Hooked_Present(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion) noexcept
{
	if (csgo->DoUnload)
		return H::SteamPresent(device, src, dest, windowOverride, dirtyRegion);


	SaveState(device);
	GUI_Init(device);
	csgo->render_device = device;
	g_Render->SetupPresent(device);
	static void* dwReturnAddress = _ReturnAddress();

	//if (dwReturnAddress == _ReturnAddress()) 
	{
		g_Render->PreRender(device);

		features->Visuals->DrawLocalShit(device);

		g_Hitmarker->Paint();

		g_Render->PostRender(device);

		g_Menu->render();

		g_Menu->watermark();

		g_Render->EndPresent(device);


		features->Eventlog->Draw();
		features->Visuals->Draw();
		if (csgo->local->isAlive()) {
			CGrenadePrediction::Get().Paint();
		}


	}
	RestoreState(device);
	return H::SteamPresent(device, src, dest, windowOverride, dirtyRegion);//return present here

}

static HRESULT D3DAPI Hooked_Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{
	if (csgo->DoUnload)
		return H::SteamReset(device, params);

	c_renderer::invalidate_device_objects();
	g_Render->InvalidateObjects();
	
	auto hr = H::SteamReset(device, params);
	if (hr >= 0) {
		c_renderer::init_device_objects(device);
		g_Render->CreateObjects(device);
	}
	return hr;

}