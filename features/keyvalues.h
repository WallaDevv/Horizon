#pragma once
DWORD get_call_address(DWORD address) {
	return (*(PDWORD)(address + 1)) + address + 5;
}

class KeyValues 
{
public:
	~KeyValues() { interfaces.memalloc->Free(this); }
	
	KeyValues* Init(const char* name) {
		static auto InitFn = (KeyValues*(__thiscall*)(void*, const char*))(get_call_address(*csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::keyvalue_init::s().c_str())].c_str())));
		return InitFn(this, name);
	};

	bool LoadFromBuffer(char const* name, const char* buffer) {
		static auto LoadFromBufferFn = (bool(__thiscall*)(KeyValues*, const char*, const char*, void*, const char*, void*, void*))(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::load_from_buffer::s().c_str())].c_str()));
		return LoadFromBufferFn(this, name, buffer, 0, 0, 0, 0);
	}

	KeyValues* FindKey(const char *name, bool create = false) {
		static auto FindKeyFn = reinterpret_cast<KeyValues*(__thiscall*)(void*, const char*, bool)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::find_key::s().c_str())].c_str()));
		return FindKeyFn(this, name, create);
	}

	const char* GetString(const char* name, const char *default_value) {
		static auto GetStringFn = reinterpret_cast<const char*(__thiscall*)(void*, const char*, const char*)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::get_string::s().c_str())].c_str()));
		return GetStringFn(this, name, default_value);
	}

	void SetString(const char* value) {
		static auto SetStringFn = reinterpret_cast<void(__thiscall*)(void*, const char*)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::set_string::s().c_str())].c_str()));
		SetStringFn(this, value);
	}

	void SetUint64(const char* name, int value1, int value2) {
		static auto SetUint64Fn = reinterpret_cast<void(__thiscall*)(void*, const char*, int, int)>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::set_uint64::s().c_str())].c_str()));
		SetUint64Fn(this, name, value1, value2);
	}
};

__forceinline static KeyValues* CreateKV(const char* name) {
	KeyValues* kv = (KeyValues*)interfaces.memalloc->Alloc(36);
	if (kv) kv->Init(name);
	return kv;
}