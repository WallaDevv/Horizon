#pragma once

class CSharedObjectTypeCache {
public:
	void AddObject(void* obj) {
		typedef void(__thiscall* tOriginal)(void*, void*);
		call_v_func<tOriginal>(this, 1)(this, obj);
	}

	void RemoveObject(void* obj) {
		typedef void(__thiscall* tOriginal)(void*, void*);
		call_v_func<tOriginal>(this, 3)(this, obj);
	}

	std::vector<CEconItem*> GetEconItems() {
		std::vector<CEconItem*> ret;
		auto size = *reinterpret_cast<size_t*>(this + 0x18);
		auto data = *reinterpret_cast<uintptr_t**>(this + 0x4);
		for (size_t i = 0; i < size; i++)
			ret.push_back(reinterpret_cast<CEconItem*>(data[i]));
		return ret;
	}
};

class CSInventoryManager {
public:
	C_EconItemView* FindOrCreateReferenceEconItem(int64_t ID) {
		static auto fnFindOrCreateReferenceEconItem = reinterpret_cast<C_EconItemView*(__thiscall*)(void*, int64_t)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::find_or_create_reference_econ_item::s().c_str())].c_str()));
		return fnFindOrCreateReferenceEconItem(this, ID);
	}

	bool EquipItemInLoadout(int team, int slot, unsigned long long itemID) {
		static auto oEquipItemInLoadout = (bool(__thiscall*)(void*, int, int, unsigned long long, bool))(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::equip_item_in_loadout::s().c_str())].c_str()));
		return oEquipItemInLoadout(this, team, slot, itemID, true);
	}
};

CSInventoryManager* i_InventoryManager;

class CPlayerInventory {
public:
	void RemoveItem(uint64_t ID) {
		static auto fnRemoveItem = reinterpret_cast<int(__thiscall*)(void*, int64_t)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::remove_item::s().c_str())].c_str()));
		fnRemoveItem(this, ID);
	}

	void RemoveItem(CEconItem* item) {
		RemoveItem(*item->GetItemID());
		GetBaseTypeCache()->RemoveObject(item);
	}

	void ClearInventory() {
		auto BaseTypeCache = this->GetBaseTypeCache();
		auto items = BaseTypeCache->GetEconItems();
		for (auto item : items) {
			RemoveItem(*item->GetItemID());
			BaseTypeCache->RemoveObject(item);
		}
	}

	CSharedObjectTypeCache* GetBaseTypeCache() {
		static auto g_GCClientSystem = **reinterpret_cast<uintptr_t**>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::g_c_client_system::s().c_str())].c_str()) + 0x2);
		static auto fnGCSDK_CGCClient_FindSOCache = reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t, uint64_t, uint64_t, bool)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::g_c_client_system_find_so_cahce::s().c_str())].c_str()));
		static auto fnGCSDK_CSharedObjectCache_CreateBaseTypeCache = reinterpret_cast<CSharedObjectTypeCache*(__thiscall*)(uintptr_t, int)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::g_c_client_system_create_byte::s().c_str())].c_str()));
		auto SOCahce = fnGCSDK_CGCClient_FindSOCache(g_GCClientSystem + 0x70, *reinterpret_cast<uint64_t*>(this + 0x8), *reinterpret_cast<uint64_t*>(this + 0x10), 0);
		return fnGCSDK_CSharedObjectCache_CreateBaseTypeCache(SOCahce, 1);
	}

	uint32_t GetSteamID() {
		return *reinterpret_cast<uint32_t*>(this + 0x8);
	}

	CUtlVector<C_EconItemView*>* GetInventoryItems() {
		return reinterpret_cast<CUtlVector<C_EconItemView*>*>(this + 0x2C);
	}

	C_EconItemView* GetInventoryItemByItemID(int64_t ID) {
		static auto fnGetInventoryItemByItemID = reinterpret_cast<C_EconItemView*(__thiscall*)(void*, int64_t)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::get_inventory_item_by_index::s().c_str())].c_str()));
		auto econ = fnGetInventoryItemByItemID(this, ID);
		if (!econ || !*(BYTE*)((uintptr_t)econ + 0x204)) return i_InventoryManager->FindOrCreateReferenceEconItem(ID);
		else return econ;
	}

	bool AddEconItem(CEconItem* item, int a3, int a4, char a5) {
		static auto fnAddEconItem = reinterpret_cast<C_EconItemView*(__thiscall*)(void*, CEconItem*, int, int, char)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::add_econ_item::s().c_str())].c_str()));
		GetBaseTypeCache()->AddObject(item);
		auto ret = fnAddEconItem(this, item, a3, a4, a5);
		if (ret) {
			auto i = GetInventoryItemByItemID(*item->GetItemID());
			*reinterpret_cast<bool*>((uintptr_t)i + 0xA1) = 1;
		}
		return ret;
	}

	C_EconItemView* GetItemInLoadout(int team, int slot) {
		typedef C_EconItemView*(__thiscall* tOriginal)(void*, unsigned int, signed int);
		return call_v_func<tOriginal>(this, 8)(this, team, slot);
	}
};

CPlayerInventory* i_LocalPlayerInventory;

void EquipItem(uint64_t id, int team) {
	auto ItemView = i_LocalPlayerInventory->GetInventoryItemByItemID(id);
	if (!ItemView) {
		ItemView = i_InventoryManager->FindOrCreateReferenceEconItem(id);
		if (!ItemView) return;
	}
	auto ItemStaticData = ItemView->GetStaticData();
	if (!ItemStaticData) return;
	i_InventoryManager->EquipItemInLoadout(team, ItemStaticData->GetEquippedPosition(), id);
}


