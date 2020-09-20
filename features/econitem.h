#pragma once
#include "econitemdefinition.h"
enum ItemQuality {
	ITEM_QUALITY_DEFAULT,
	ITEM_QUALITY_GENUINE,
	ITEM_QUALITY_VINTAGE,
	ITEM_QUALITY_UNUSUAL,
	ITEM_QUALITY_SKIN,
	ITEM_QUALITY_COMMUNITY,
	ITEM_QUALITY_DEVELOPER,
	ITEM_QUALITY_SELFMADE,
	ITEM_QUALITY_CUSTOMIZED,
	ITEM_QUALITY_STRANGE,
	ITEM_QUALITY_COMPLETED,
	ITEM_QUALITY_UNK2,
	ITEM_QUALITY_TOURNAMENT
};
enum ItemRarity {
	ITEM_RARITY_DEFAULT,
	ITEM_RARITY_COMMON,
	ITEM_RARITY_UNCOMMON,
	ITEM_RARITY_RARE,
	ITEM_RARITY_MYTHICAL,
	ITEM_RARITY_LEGENDARY,
	ITEM_RARITY_ANCIENT,
	ITEM_RARITY_IMMORTAL
};



static uintptr_t GetItemSchema() {
	static auto fnGetItemSchema = reinterpret_cast<uintptr_t(__stdcall*)()>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::get_item_schema::s().c_str())].c_str()));
	return fnGetItemSchema();
}

class CEconItem {
	unsigned short* GetEconItemData() {
		return reinterpret_cast<unsigned short*>(this + 0x26);
	}

	void UpdateEquippedState(unsigned int state) {
		static auto fnUpdateEquippedState = reinterpret_cast<int(__thiscall*)(CEconItem*, unsigned int)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::update_equipped_state::s().c_str())].c_str()));
		fnUpdateEquippedState(this, state);
	}

public:
	uint32_t * GetInventory() {
		return reinterpret_cast<uint32_t*>(this + 0x20);
	}

	uint32_t* GetAccountID() {
		return reinterpret_cast<uint32_t*>(this + 0x1C);
	}

	uint16_t* GetDefIndex() {
		return reinterpret_cast<uint16_t*>(this + 0x24);
	}

	uint64_t* GetItemID() {
		return reinterpret_cast<uint64_t*>(this + 0x8);
	}

	uint64_t* GetOriginalID() {
		return reinterpret_cast<uint64_t*>(this + 0x10);
	}

	unsigned char* GetFlags() {
		return reinterpret_cast<unsigned char*>(this + 0x30);
	}

	void SetQuality(ItemQuality quality) {
		auto data = *GetEconItemData();
		*GetEconItemData() = data ^ (data ^ 32 * quality) & 0x1E0;
	}

	void SetRarity(ItemRarity rarity) {
		auto data = *GetEconItemData();
		*GetEconItemData() = (data ^ (rarity << 11)) & 0x7800 ^ data;
	}

	void SetOrigin(int origin) {
		auto data = *GetEconItemData();
		*GetEconItemData() = data ^ ((unsigned __int8)data ^ (unsigned __int8)origin) & 0x1F;
	}

	void SetLevel(int level) {
		auto data = *GetEconItemData();
		*GetEconItemData() = data ^ (data ^ (level << 9)) & 0x600;
	}

	void SetInUse(bool in_use) {
		auto data = *GetEconItemData();
		*GetEconItemData() = data & 0x7FFF | (in_use << 15);
	}

	void SetCustomName(const char* name) {
		static auto Address = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::set_custom_name::s().c_str())].c_str());
		static auto fnSetCustomName = reinterpret_cast<CEconItem*(__thiscall*)(void*, const char*)>(*reinterpret_cast<uintptr_t*>(Address + 1) + Address + 5);
		fnSetCustomName(this, name);
	}

	void SetCustomDesc(const char* name) {
		static auto Address = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::set_custom_desc::s().c_str())].c_str());
		static auto fnSetCustomDesc = reinterpret_cast<CEconItem*(__thiscall*)(void*, const char*)>(*reinterpret_cast<uintptr_t*>(Address + 1) + Address + 5);
		fnSetCustomDesc(this, name);
	}

	void SetPaintSeed(float seed) {
		SetAttributeValue<float>(7, seed);
	}

	void SetPaintKit(float kit) {
		SetAttributeValue<float>(6, kit);
	}

	void SetPaintWear(float wear) {
		SetAttributeValue<float>(8, wear);
	}

	void SetStatTrak(int val) {
		SetAttributeValue<int>(80, val);
		SetAttributeValue<int>(81, 0);
		SetQuality(ITEM_QUALITY_STRANGE);
	}

	void AddSticker(int index, int kit, float wear, float scale, float rotation) {
		SetAttributeValue<int>(113 + 4 * index, kit);
		SetAttributeValue<float>(114 + 4 * index, wear);
		SetAttributeValue<float>(115 + 4 * index, scale);
		SetAttributeValue<float>(116 + 4 * index, rotation);
	}

	template<typename TYPE>
	void SetAttributeValue(int index, TYPE val) {
		auto v15 = (DWORD*)GetItemSchema();
		auto v16 = *(DWORD *)(v15[72] + 4 * index);
		static auto fnSetDynamicAttributeValue = reinterpret_cast<int(__thiscall*)(CEconItem*, DWORD, void*)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::set_attribute_value::s().c_str())].c_str()));
		fnSetDynamicAttributeValue(this, v16, &val);
	}
};

class C_EconItemView {
public:
	CEconItemDefinition* GetStaticData() {
		static auto fnGetStaticData = reinterpret_cast<CEconItemDefinition*(__thiscall*)(void*)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::get_static_data::s().c_str())].c_str()));
		return fnGetStaticData(this);
	}
	CEconItem* GetSOCData() {
		static auto fnGetSOCData = reinterpret_cast<CEconItem*(__thiscall*)(C_EconItemView*)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::set_soc_data::s().c_str())].c_str()));
		return fnGetSOCData(this);
	}
};

static CEconItem* CreateEconItem() {
	static auto fnCreateSharedObjectSubclass_EconItem_ = reinterpret_cast<CEconItem*(__stdcall*)()>(*reinterpret_cast<uintptr_t*>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::create_econ_item::s().c_str())].c_str()) + 3));
	return fnCreateSharedObjectSubclass_EconItem_();
}