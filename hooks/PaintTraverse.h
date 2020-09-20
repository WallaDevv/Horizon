#include "../features/Features.h"

static int ScreenSize2W, ScreenSize2H;



void __stdcall Hooked_PaintTraverse(unsigned int vguiPanel, bool forceRepaint, bool allowForce);




float __fastcall Hooked_GetScreenAspectRatio(void* pEcx, void* pEdx, int32_t iWidth, int32_t iHeight);

_declspec(noinline) const char* GetForeignFallbackFontName_Detour(uintptr_t* ecx);


const char* __fastcall Hooked_GetForeignFallbackFontName(uintptr_t* ecx, uint32_t);
