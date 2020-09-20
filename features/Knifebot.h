#pragma once
#include "../hooks/Hooks.h"

class CKnifebot : public Singleton<CKnifebot> {
public:
	void Run();
};