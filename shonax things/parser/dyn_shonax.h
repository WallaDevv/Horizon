#pragma once


#include <unordered_map>
#include <set>
#include <algorithm>
struct skinInfo
{
	int seed = -1;
	int paintkit;
	std::wstring tagName;
};




inline static std::unordered_map<std::wstring, std::set<std::wstring>> weaponSkins;
inline static std::unordered_map<std::wstring, skinInfo> skinMap;
inline static std::unordered_map<std::wstring, std::wstring> skinNames;
