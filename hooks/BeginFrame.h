#pragma once
#include "Hooks.h"

void __fastcall Hooked_BeginFrame(void* thisptr)
{
	
	features->BulletTracer->Draw();
	H::BeginFrame(thisptr);
}