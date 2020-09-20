#pragma once

class CEconItemDefinition {
public:
	KeyValues* GetRawDefinition() {
		typedef KeyValues*(__thiscall* oOriginal)(PVOID);
		return call_v_func<oOriginal>(this, 10)(this);
	}

	const char* GetItemType() {
		return *reinterpret_cast<const char**>((uintptr_t)this + 0x54);
	}

	const char* GetModelName() {
		return *reinterpret_cast<const char**>((uintptr_t)this + 0x94);
	}

	const char* GetWorldModelName() {
		return *reinterpret_cast<const char**>((uintptr_t)this + 0x9C);
	}

	const char* GetInventoryImage() {
		return (const char*)(*(int(__thiscall **)(int))(*(DWORD*)(DWORD)this + 0x14))((DWORD)this);
	}

	std::string GetWeaponLocalizeName() {
		auto wname = *reinterpret_cast<const char**>((uintptr_t)this + 0x4C);
		const auto wide_name = interfaces.localize->find(wname);
		char name[256];
		sprintf(name, u8"%ls", wide_name);
		return std::string(name);
	}

	const char* GetWeaponName() {
		return *reinterpret_cast<const char**>((uintptr_t)this + 0x1C0);
	}

	int GetRarityValue() {
		return *reinterpret_cast<char*>((uintptr_t)this + 0x2A);
	}

	int GetEquippedPosition() {
		return *reinterpret_cast<int*>((uintptr_t)this + 0x24C);
	}

	int GetNumSupportedStickerSlots() {
		return *reinterpret_cast<int*>((uintptr_t)this + 0x108);
	}

	int GetDefinitionIndex() {
		return *reinterpret_cast<int*>((uintptr_t)this + 0x8);
	}

	std::vector<uint16_t> GetAssociatedItems() {
		std::vector<uint16_t> items;
		auto size = *reinterpret_cast<int*>((uintptr_t)this + 0x18);
		auto data = *reinterpret_cast<uintptr_t*>((uintptr_t)this + 0xC);
		if (data) {
			for (int i = 0; i < size; i++)
				items.push_back(*reinterpret_cast<uint16_t*>(data + (i * sizeof(uint16_t))));
		}
		return items;
	}
};