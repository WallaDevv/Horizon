#pragma once

class CEconPaintKitDefinition {
public:
	std::string GetSkinLocalizeName() {
		auto wname = *reinterpret_cast<const char**>((uintptr_t)this + 0x24);
		const auto wide_name = interfaces.localize->find(wname);
		char name[256];
		sprintf(name, u8"%ls", wide_name);
		return std::string(name);
	}

	int GetPaintKit() {
		return *reinterpret_cast<int*>((uintptr_t)this);
	}

	int GetRarityValue() {
		return *reinterpret_cast<int*>((uintptr_t)this + 0x68);
	}

	const char* GetSkinName() {
		return *reinterpret_cast<const char**>((uintptr_t)this + 0x4);
	}
};

