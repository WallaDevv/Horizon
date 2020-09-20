#pragma once
#include <vector>
#include <map>
/*
struct Attributes {
	int def_index;
	int skin;
	char name[32];
	int st;
	int seed;
	float wear;
	int sticker[4];
};

namespace inventory_changer {
	std::vector<uint64_t> added_items;
	std::map<uint64_t, Attributes> item_attributes;

	int equip_wearables(IBaseCombatWeapon* wearable, IBasePlayer* local) {
		static auto fn_equip = reinterpret_cast<int(__thiscall*)(void*, void*)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::equip_wearables::s().c_str())].c_str()));
		return fn_equip(wearable, local);
	}

	int initialize_attributes(IBaseCombatWeapon* wearable) {
		static auto fn_initialize_attributes = reinterpret_cast<int(__thiscall*)(void*)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::initialize_attributes::s().c_str())].c_str()));
		return fn_initialize_attributes(wearable);
	}

	static auto get_wearable_create_fn() -> CreateClientClassFn {
		auto p_class = interfaces.client->GetAllClasses();
		while (p_class->m_ClassID != ClassId->CEconWearable) p_class = p_class->m_pNext;
		return p_class->m_pCreateFn;
	}

	void pdu_inventory_gloves(IBasePlayer* local) {
		if (local->GetHealth() < 1) return;
		if (local->is_controlling_bot()) return;

		player_info_t info;
		if (!interfaces.engine->GetPlayerInfo(interfaces.engine->GetLocalPlayer(), &info)) return;

		auto wearables = local->get_wearables();
		if (wearables) {
			auto wearable = reinterpret_cast<IBaseCombatWeapon*>(interfaces.ent_list->GetClientEntityFromHandle(wearables[0]));
			if (!wearable) {
				auto econ_skin_data = i_LocalPlayerInventory->GetItemInLoadout(local->GetTeam(), 41);
				if (econ_skin_data) {
					//Create
					auto entry = interfaces.ent_list->GetHighestEntityIndex() + 1;
					for (auto i = 0; i < interfaces.ent_list->GetHighestEntityIndex(); i++) {
						auto entity = interfaces.ent_list->GetClientEntity(i);
						if (entity && entity->GetClientClass()->m_ClassID == ClassId->CRopeKeyframe) {
							entry = i;
							break;
						}
					}
					const auto serial = Math::RandomInt(1, 0x1000);
					const auto create_wearable_fn = get_wearable_create_fn();
					create_wearable_fn(entry, serial);

					//Change
					wearable = reinterpret_cast<IBaseCombatWeapon*>(interfaces.ent_list->GetClientEntity(entry));
					if (wearable) {
						wearables[0] = entry | serial << 16;
						auto new_econ_data = econ_skin_data->GetSOCData();
						if (!new_econ_data) return;
						auto new_weapon_definition = econ_skin_data->GetStaticData();
						if (!new_weapon_definition) return;

						auto id = *new_econ_data->GetItemID();
						uint64_t mask = (std::numeric_limits<uint32_t>::max)();
						uint32_t low_id = mask & id;
						uint32_t high_id = id >> 32;

						wearable->get_item_id_low() = low_id;
						wearable->get_item_id_high() = high_id;
						wearable->get_item_definition_index() = *new_econ_data->GetDefIndex();
						wearable->get_account_id() = info.xuidLow;
						wearable->is_initialized() = true;

						wearable->SetGloveModelIndex(interfaces.models.model_info->GetModelIndex(new_weapon_definition->GetWorldModelName()));
						equip_wearables(wearable, local);
						local->get_body() = 1;
						initialize_attributes(wearable);
						interfaces.leafsystem->create_renderable_handle(wearable);
					}
				}
			}
		}
	}

      void pdu_inventory_items(IBasePlayer* local) {
		if (local->GetHealth() < 1) return;
		if (local->is_controlling_bot()) return;

		player_info_t info;
		if (!interfaces.engine->GetPlayerInfo(interfaces.engine->GetLocalPlayer(), &info)) return;

		auto weapons = local->GetWeapons();
		for (auto weapon : weapons) {
			
			if (!weapon) continue;

			if (info.xuidLow != weapon->OwnerXuidLow()) continue;
			if (info.xuidHigh != weapon->OwnerXuidHigh()) continue;

			auto econ_item_view = weapon->get_econ_item_view();
			if (!econ_item_view) continue;

			auto econ_item_definition = econ_item_view->GetStaticData();
			if (!econ_item_definition) continue;

			auto new_econ_item_view = i_LocalPlayerInventory->GetItemInLoadout(local->GetTeam(), econ_item_definition->GetEquippedPosition());
			if (!new_econ_item_view) continue;

			auto new_econ_skin_data = new_econ_item_view->GetSOCData();
			if (!new_econ_skin_data) continue;

			auto id = *new_econ_skin_data->GetItemID();
			uint64_t mask = (std::numeric_limits<size_t>::max)();
			uint32_t low_id = mask & id;
			uint32_t high_id = id >> 32;
			weapon->get_item_id_low() = low_id;
			weapon->get_item_id_high() = high_id;		
			weapon->get_account_id() = info.xuidLow;

			if (weapon->IsKnife()) {
				weapon->get_item_definition_index() = *new_econ_skin_data->GetDefIndex();
				auto new_econ_item_definition = new_econ_item_view->GetStaticData();
				if (new_econ_item_definition) {
					vars.skins.icon = sParser::GetWeaponInfo(new_econ_item_definition->GetDefinitionIndex())->icon;
					weapon->ModelIndex() = interfaces.models.model_info->GetModelIndex(new_econ_item_definition->GetModelName());
				}			
			}						
		}

		auto local_weapon = local->GetWeapon();
		if (!local_weapon) return;
		
		if (!local_weapon->IsKnife()) return;

		auto view_model = local->GetViewModel();
		if (!view_model) return;
		auto world_model = local_weapon->m_hWeaponWorldModel();
		if (!world_model) return;


		auto view_model_weapon_new_econ_skin_data = i_LocalPlayerInventory->GetItemInLoadout(local->GetTeam(), 0);
		if (!view_model_weapon_new_econ_skin_data) return;
		auto view_model_new_econ_item_definition = view_model_weapon_new_econ_skin_data->GetStaticData();
		if (!view_model_new_econ_item_definition) return;

		view_model->SetModelIndex(interfaces.models.model_info->GetModelIndex(view_model_new_econ_item_definition->GetModelName()));
		world_model->ModelIndex() == interfaces.models.model_info->GetModelIndex(view_model_new_econ_item_definition->GetWorldModelName());
	}

	void add_item_to_inventory(int def_index, int paint_kit, int seed, float wear, int stat_trak, int sticker1, int sticker2, int sticker3, int sticker4, bool equip_ct, bool equip_tt, bool first = false) {
		//Create item
		auto item = CreateEconItem();
		//Set main settings
		*item->GetAccountID() = i_LocalPlayerInventory->GetSteamID();
		*item->GetDefIndex() = def_index;
		srand(time(0));

		//Create itemID
		uint64_t ItemID = Math::RandomInt(20000, 50000000);

		//Set other
		*item->GetItemID() = ItemID;
		*item->GetInventory() = first ? 0 : 1;
		*item->GetFlags() = 0;
		*item->GetOriginalID() = 0;

		//Set attributes
		item->SetPaintKit(paint_kit);
		item->SetPaintSeed(seed);
		item->SetPaintWear(wear);
		item->SetOrigin(8);

		if (stat_trak > 0) item->SetStatTrak(stat_trak);
		if (def_index == 59  || def_index == 41 || def_index == 24 || (def_index>=500 && def_index<=530) /*|| ItemDefinitionIndex(def_index).is_gloves()*///) {
	/*		item->SetRarity(ItemRarity(6));
			item->SetQuality(/*ItemDefinitionIndex(def_index).is_gloves() ? ItemQuality(4) :*/// ItemQuality(3));
	/*	} else item->SetRarity(ItemRarity(sParser::GetSkinRarity(def_index, paint_kit)));

		item->SetLevel(1);
		item->SetInUse(true);
		item->AddSticker(0, sticker1, 0, 1, 1);
		item->AddSticker(1, sticker2, 0, 1, 1);
		item->AddSticker(2, sticker3, 0, 1, 1);
		item->AddSticker(3, sticker4, 0, 1, 1);
		
		//Save attributes for work with items
		added_items.push_back(ItemID);
		item_attributes[ItemID].def_index = def_index;
		item_attributes[ItemID].st = stat_trak;
		item_attributes[ItemID].skin = paint_kit;
		item_attributes[ItemID].seed = seed;
		item_attributes[ItemID].wear = wear;
		item_attributes[ItemID].sticker[0] = sticker1;
		item_attributes[ItemID].sticker[1] = sticker2;
		item_attributes[ItemID].sticker[2] = sticker3;
		item_attributes[ItemID].sticker[3] = sticker4;

		//Add item to inventory
		i_LocalPlayerInventory->AddEconItem(item, 1, 0, 1);
		#define CS_TEAM_CT 3
		#define CS_TEAM_TT 2
		//Equip item if needed
		if (equip_ct) EquipItem(*item->GetItemID(), CS_TEAM_CT);
		if (equip_tt) EquipItem(*item->GetItemID(), CS_TEAM_TT);
	}




	bool is_item_equiped(IBaseCombatWeapon* weapon, int itemID, int teamid);

	*/


	/*



	void remove_item(int index) {
		uint64_t itemid = added_items[index];
		item_attributes[itemid].skin = 0;
		item_attributes[itemid].st = 0;
		item_attributes[itemid].seed = 0;
		item_attributes[itemid].wear = 0;
		item_attributes[itemid].sticker[0] = 0;
		item_attributes[itemid].sticker[1] = 0;
		item_attributes[itemid].sticker[2] = 0;
		item_attributes[itemid].sticker[3] = 0;
		i_LocalPlayerInventory->RemoveItem(itemid);
		added_items.erase(added_items.begin() + index);
	}

	string get_guns_string() {
		string str;
		for (auto& item : added_items) {
			if (item_attributes[item].def_index == 0) {
				str += "None";
			} else if (std::strstr(item_attributes[item].name, "")) {
				str += sParser::WeaponNames[sParser::WeaponIdToItemId(item_attributes[item].def_index)].name;
			} else {
				str.push_back('\"');
				str += item_attributes[item].name;
				str.push_back('\"');
			}
			str.push_back('\0');
		}
		str.push_back('\0');
		str.push_back('\0');
		return str;
	}

	bool is_item_equipped(int equippos, uint64_t itemid, int teamid) {
		auto loadout_item = i_LocalPlayerInventory->GetItemInLoadout(teamid, equippos);
		if (!loadout_item) return false;
		auto soc_data = loadout_item->GetSOCData();
		if (!soc_data) return false;
		if (*soc_data->GetItemID() == itemid) return true;
		return false;
	}

}

*/