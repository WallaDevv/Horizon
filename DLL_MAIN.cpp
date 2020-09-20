//#include "protect/protect.hpp"
#include "DLL_MAIN.h"
#include "hooks/Hooks.h"
#include "netvar_manager.h"
#include "License.h"
#include "render.h"
#include <thread>
#include <chrono>
#include "console/console.hpp"



#pragma comment( lib, "Winhttp.lib" )
#pragma comment( lib, "ws2_32.lib" )




CNetVarManager netvars;

void InitializeNetvars() {
	netvars.Initialize();
}


DWORD WINAPI CheatMain(LPVOID lpThreadParameter)
{

	while (!(csgo->Init.Window = FindWindowA(hs::Valve001::s().c_str(), NULL)))
		this_thread::sleep_for(200ms);
	while (!GetModuleHandleA(hs::serverbrowser_dll::s().c_str()))
		this_thread::sleep_for(200ms);
	while (!GetModuleHandleA(hs::client_dll::s().c_str()))
		this_thread::sleep_for(200ms);
	while (!GetModuleHandleA(hs::engine_dll::s().c_str()))
		this_thread::sleep_for(200ms);
	//console::initialize("nightware by uwurgan, deliorix, hineh");
	srand(time(0));

	I::Setup();
	InitializeNetvars();
	H::Hook();
	
	while (!csgo->DoUnload)
		this_thread::sleep_for(1s);
	
	if (csgo->DoUnload)
		H::UnHook();

	
	interfaces.engine->ClientCmd_Unrestricted(hs::clear::s().c_str(), 0);
	interfaces.engine->ClientCmd_Unrestricted(hs::unload_message::s().c_str(), 0);
	this_thread::sleep_for(3s);
	SetWindowLongPtr(csgo->Init.Window, GWL_WNDPROC, (LONG_PTR)csgo->Init.OldWindow);
	FreeLibraryAndExitThread(static_cast<HMODULE>(csgo->Init.Dll), 0);
	return 0;
}






BOOL APIENTRY DllMain(HMODULE hDll, DWORD dwReason, LPVOID lpThreadParameter) {
	if (dwReason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hDll);
		csgo->Init.Dll = hDll;
		CreateThread(nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(CheatMain), lpThreadParameter, 0, nullptr);
	}
	return TRUE;
}

