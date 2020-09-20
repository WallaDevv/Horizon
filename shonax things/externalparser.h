#pragma once
#include <map>
#include <vector>






/*

namespace web_parse {
	std::string ParseString(std::string szBefore, std::string szAfter, std::string szSource) {
		if (!szBefore.empty() && !szAfter.empty() && !szSource.empty() && (szSource.find(szBefore) != std::string::npos) && (szSource.find(szAfter) != std::string::npos)) {
			std::string t = strstr(szSource.c_str(), szBefore.c_str());
			t.erase(0, szBefore.length());
			std::string::size_type loc = t.find(szAfter, 0);
			t = t.substr(0, loc);
			return t;
		}
		else return "";
	}

	string HttpGet(string szHost, string szApi) {
		string request;
		string response;
		int resp_leng;
		char buffer[1024];
		struct sockaddr_in serveraddr;
		int sock;
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {}
		struct hostent* host;
		host = gethostbyname(szHost.c_str());
		int port = 80;
		request += "GET " + szApi + " HTTP/1.0\r\n";
		request += "Host: " + szHost + "\r\n";
		request += "User-Agent: WebParser [Build: 1.1] \r\n";
		request += "\r\n";
		if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {}
		memset(&serveraddr, 0, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
		serveraddr.sin_port = htons((unsigned short)port);
		struct in_addr** addr_list;
		addr_list = (struct in_addr**) host->h_addr_list;
		response = "";
		if (connect(sock, (struct sockaddr*) & serveraddr, sizeof(serveraddr)) < 0) {}
		if (send(sock, request.c_str(), request.length(), 0) != request.length()) {}
		resp_leng = 1024;
		while (resp_leng > 0) {
			resp_leng = recv(sock, (char*)&buffer, 1024, 0);
			if (resp_leng > 0) response += string(buffer).substr(0, resp_leng);
		}
		closesocket(sock);
		WSACleanup();
		return response;
	}


}



namespace sParser {
	struct weapon_info {
		constexpr weapon_info(const char* model, const char* icon = nullptr, int animindex = -1) : model(model), icon(icon), animindex(animindex) {}
		const char* model;
		const char* icon;
		int animindex;
	};

	struct weapon_name {
		constexpr weapon_name(const int definition_index, const char* name) : definition_index(definition_index), name(name) {}
		int definition_index = 0;
		const char* name = nullptr;
	};

	const weapon_info* GetWeaponInfo(int defindex) {
		const static std::map<int, weapon_info> Info = {
			{ WEAPON_KNIFE,{ "models/weapons/v_knife_default_ct.mdl", "knife_default_ct", 2 } },
			{ WEAPON_KNIFE_T,{ "models/weapons/v_knife_default_t.mdl", "knife_t", 12 } },
			{ WEAPON_KNIFE_BAYONET,{ "models/weapons/v_knife_bayonet.mdl", "bayonet", 0 } },
			{ WEAPON_KNIFE_FLIP,{ "models/weapons/v_knife_flip.mdl", "knife_flip", 4 } },
			{ WEAPON_KNIFE_GUT,{ "models/weapons/v_knife_gut.mdl", "knife_gut", 5 } },
			{ WEAPON_KNIFE_KARAMBIT,{ "models/weapons/v_knife_karam.mdl", "knife_karambit", 7 } },
			{ WEAPON_KNIFE_M9_BAYONET,{ "models/weapons/v_knife_m9_bay.mdl", "knife_m9_bayonet", 8 } },
			{ WEAPON_KNIFE_TACTICAL,{ "models/weapons/v_knife_tactical.mdl", "knife_tactical" } },
			{ WEAPON_KNIFE_FALCHION,{ "models/weapons/v_knife_falchion_advanced.mdl", "knife_falchion", 3 } },
			{ WEAPON_KNIFE_SURVIVAL_BOWIE,{ "models/weapons/v_knife_survival_bowie.mdl", "knife_survival_bowie", 11 } },
			{ WEAPON_KNIFE_BUTTERFLY,{ "models/weapons/v_knife_butterfly.mdl", "knife_butterfly", 1 } },
			{ WEAPON_KNIFE_PUSH,{ "models/weapons/v_knife_push.mdl", "knife_push", 9 } },
			{ WEAPON_KNIFE_URSUS,{ "models/weapons/v_knife_ursus.mdl", "knife_ursus", 13 } },
			{ WEAPON_KNIFE_GYPSY_JACKKNIFE,{ "models/weapons/v_knife_gypsy_jackknife.mdl", "knife_gypsy_jackknife", 6 } },
			{ WEAPON_KNIFE_STILETTO,{ "models/weapons/v_knife_stiletto.mdl", "knife_stiletto", 10 } },
			{ WEAPON_KNIFE_WIDOWMAKER,{ "models/weapons/v_knife_widowmaker.mdl", "knife_widowmaker", 14 } },
			{ GLOVE_STUDDED_BLOODHOUND,{ "models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl" } },
			{ GLOVE_T_SIDE,{ "models/weapons/v_models/arms/glove_fingerless/v_glove_fingerless.mdl" } },
			{ GLOVE_CT_SIDE,{ "models/weapons/v_models/arms/glove_hardknuckle/v_glove_hardknuckle.mdl" } },
			{ GLOVE_SPORTY,{ "models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl" } },
			{ GLOVE_SLICK,{ "models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl" } },
			{ GLOVE_LEATHER_WRAP,{ "models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl" } },
			{ GLOVE_MOTORCYCLE,{ "models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl" } },
			{ GLOVE_SPECIALIST,{ "models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl" } },
			{ GLOVE_HYDRA,{ "models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound_hydra.mdl" } },
			{  521, { "models/weapons/v_knife_outdoor.mdl", "knife_outdoor", 14}  },
			{  518 ,  {"models/weapons/v_knife_canis.mdl", "knife_canis",14}},
			{  517 ,  {"models/weapons/v_knife_cord.mdl", "knife_cord", 14}},
			{  525 ,  {"models/weapons/v_knife_skeleton.mdl", "knife_skeleton" ,14}},
			{  503 ,  {"models/weapons/v_knife_css.mdl", "knife_css", 14}}


		};
		const auto entry = Info.find(defindex);
		return entry == end(Info) ? nullptr : &entry->second;
	}
	
	const std::vector<weapon_name> WeaponNames = {
		{ 1, "Desert Eagle" },
		{ 2, "Dual Berettas" },
		{ 3, "Five-SeveN" },
		{ 4, "Glock-18" },
		{ 30, "Tec-9" },
		{ 32, "P2000" },
		{ 36, "P250" },
		{ 61, "USP-S" },
		{ 63, "CZ75 Auto" },
		{ 64, "R8 Revolver" },
		{ 7, "AK-47" },
		{ 8, "AUG" },
		{ 10, "FAMAS" },
		{ 13, "Galil AR" },
		{ 16, "M4A4" },
		{ 39, "SG 553" },
		{ 60, "M4A1-S" },
		{ 17, "MAC-10" },
		{ 19, "P90" },
		{ 23, "MP5-SD" },
		{ 24, "UMP-45" },
		{ 26, "PP-Bizon" },
		{ 33, "MP7" },
		{ 34, "MP9" },
		{ 14, "M249" },
		{ 25, "XM1014" },
		{ 27, "MAG-7" },
		{ 28, "Negev" },
		{ 29, "Sawed-Off" },
		{ 35, "Nova" },
		{ 9, "AWP" },	
		{ 11, "G3SG1" },
		{ 38, "SCAR-20" },	
		{ 40, "SSG 08" },	
		 
		{ WEAPON_KNIFE_BAYONET, "Bayonet" },
		{ WEAPON_KNIFE_FLIP, "Flip Knife" },
		{ WEAPON_KNIFE_GUT, "Gut Knife" },
		{ WEAPON_KNIFE_KARAMBIT, "Karambit" },
		{ WEAPON_KNIFE_M9_BAYONET, "M9 Bayonet" },
		{ WEAPON_KNIFE_TACTICAL, "Huntsman Knife" },
		{ WEAPON_KNIFE_FALCHION, "Falchion Knife" },
		{ WEAPON_KNIFE_SURVIVAL_BOWIE, "Bowie Knife" },
		{ WEAPON_KNIFE_BUTTERFLY, "Butterfly Knife" },
		{ WEAPON_KNIFE_PUSH, "Shadow Daggers" },
		{ WEAPON_KNIFE_URSUS, "Ursus Knife" },
		{ WEAPON_KNIFE_GYPSY_JACKKNIFE, "Navaja Knife" },
		{ WEAPON_KNIFE_STILETTO, "Stiletto Knife" },
		{ WEAPON_KNIFE_WIDOWMAKER, "Talon Knife" },

		{ GLOVE_STUDDED_BLOODHOUND, "Bloodhound" },
		{ GLOVE_SPORTY, "Sporty" },
		{ GLOVE_SLICK, "Slick" },
		{ GLOVE_LEATHER_WRAP, "Handwrap" },
		{ GLOVE_MOTORCYCLE, "Motorcycle" },
		{ GLOVE_SPECIALIST, "Specialist" },
		{ GLOVE_HYDRA, "Hydra" }
	};




	struct ParseStruct {
		int id;
		int rarity;
		string name;
		auto operator < (const ParseStruct& other) const -> bool {
			return name < other.name;
		}
	};

	vector<ParseStruct> SkinsSorted[5050];
	vector<ParseStruct> SkinsNosorted;
	vector<ParseStruct> Stickers;
	vector<ParseStruct> Medals;

	string WeaponsStr;
	string SkinsStr[5050];
	string StickersStr;

	void AddSortedSkin(int id, int rarity, string name, string weapons) {
		string w = "";
		for (int i = 0; i < weapons.length(); i++) {
			if (weapons[i] != ';') w = w + weapons[i];
			else {
				if (w != "") {
					if (atoi(w.c_str()) != 0) {
						SkinsSorted[atoi(w.c_str())].push_back({ id, rarity, name });
						SkinsStr[atoi(w.c_str())] += name;
						SkinsStr[atoi(w.c_str())].push_back('\0');
					}
					w = "";
				}
			}
		}
		if (w != "") {
			if (atoi(w.c_str()) != 0) {
				SkinsSorted[atoi(w.c_str())].push_back({ id, rarity, name });
				SkinsStr[atoi(w.c_str())] += name;
				SkinsStr[atoi(w.c_str())].push_back('\0');
			}
		}
	}

	bool Parse();
	int WeaponIdToItemId(int id) {
		for (int i = 0; i < WeaponNames.size(); i++) {
			if (WeaponNames[i].definition_index == id)
				return i;
		}
		return 0;
	}







	int GetWeaponRarity(int defindex) {
		switch (defindex) {
		case WEAPON_DEAGLE:
		case WEAPON_GLOCK:
		case weapon_hkp2000:
		case WEAPON_AK47:
		case WEAPON_M4A1:
		case WEAPON_M4A1S:
		case WEAPON_AWP:
		case WEAPON_USPS: return 2;
		case WEAPON_AUG:
		case WEAPON_FAMAS:
		case WEAPON_GALIL:
		case WEAPON_SG553:
		case WEAPON_G3SG1:
		case WEAPON_SCAR20:
		case WEAPON_SSG08:
		case WEAPON_REVOLVER: return 3;
		default: return 0;
		}
	}

	int GetSkinRarity(int defindex, int paintkit) {
		for (int i = 0; i < SkinsSorted[defindex].size(); i++) {
			if (SkinsSorted[defindex][i].id == paintkit)
				return min(GetWeaponRarity(defindex) + SkinsSorted[defindex][i].rarity - 1, SkinsSorted[defindex][i].rarity == 7 ? 7 : 6);
		}
		return 0;
	}

}*/