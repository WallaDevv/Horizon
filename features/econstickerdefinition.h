#pragma once

class CEconStickerDefinition {
public:
	std::string GetStickerLocalizeName() {
		auto wname = *reinterpret_cast<const char**>((uintptr_t)this + 0x28);
		const auto wide_name = g_localize->find(wname);
		char name[256];
		sprintf(name, u8"%ls", wide_name);
		return std::string(name);
	}

	int GetStickerID() {
		return *reinterpret_cast<int*>((uintptr_t)this);
	}

	int GetRarity() {
		return *reinterpret_cast<int*>((uintptr_t)this + 0x4);
	}

	int GetEventID() {
		return *reinterpret_cast<int*>((uintptr_t)this + 0x68);
	}

	int GetTeamID() {
		return *reinterpret_cast<int*>((uintptr_t)this + 0x6C);
	}

	int GetPlayerID() {
		return *reinterpret_cast<int*>((uintptr_t)this + 0x70);
	}

	const char* GetStickerName() {
		return *reinterpret_cast<const char**>((uintptr_t)this + 0x8);
	}
};

