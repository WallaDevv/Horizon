#include "Hooks.h"
#include "../netvar_manager.h"

// Include hooks
#include "PaintTraverse.h"
#include "Createmove.h"
#include "FrameStageNotify.h"
#include "EndScene.h"
#include "../WndProc.h"
#include "SceneEnd.h"
#include "DoPostScreenEffects.h"
#include "../features/FireEventClientSide.h"
#include "BeginFrame.h"
#include "OverRideView.h"
#include "DrawModelExecuted.h"
#include "SetupAnimation.h"
#include "EngineHook.h"
#include "../TraceRay.h"
#include "../detours.h"
#include "../console/console.hpp"

#include "../steam_sdk/isteamclient.h"
#include "../steam_sdk/isteamgamecoordinator.h"
#include "../steam_sdk/isteamuser.h"
#include "../steam_sdk/isteamfriends.h"


inline unsigned int get_virtual(void* _class, unsigned int index) { return static_cast<unsigned int>((*reinterpret_cast<int**>(_class))[index]); }

ClMoveFn H::ClMove;
GetForeignFallbackFontNameFn H::GetForeignFallbackFontName;
send_net_msg_fn H::SendNetMsg;
send_datagram_fn H::SendDatagram;
SetupBonesFn H::SetupBones;
temp_entities_fn H::TempEntities;
ClientEffectCallback H::ImpactCallback;
CClientEffectRegistration* H::Effects;
DoExtraBoneProcessingFn H::DoExtraBoneProcessing;
StandardBlendingRulesFn H::StandardBlendingRules;
ShouldSkipAnimFrameFn H::ShouldSkipAnimFrame;
UpdateClientSideAnimationFn H::UpdateClientSideAnimation;
Present_t H::SteamPresent;
Reset_t H::SteamReset;
BuildTransformationsFn H::BuildTransformations;
PaintTraverseFn H::PaintTraverse;
SvCheatsGetBoolFn H::SvCheatsGetBool;
SvCheatsGetBoolFn H::SvCheatsGetBool1;
ListLeavesInBoxFn H::ListLeavesInBox;
RunCommandFn H::RunCommand;
InPredictionFn H::InPrediction;
FrameStageNotifyFn H::FrameStageNotify;
WriteUsercmdDeltaToBufferFn H::WriteUsercmdDeltaToBuffer;
OverRideViewFn H::OverRideView;
GetViewModelFOVFn H::GetViewModelFOV;
DoPostScreenEffectsFn H::DoPostScreenEffects;
BoxVisibleFn H::BoxVisible;
IsHLTVFn H::IsHLTV;
AspectRatioFn H::AspectRatio;
BeginFrameFn H::BeginFrame;
fnDrawModel H::DrawModel;
namespace Index
{
	// client mode interface
	const int CreateMove = 24;
	const int OverrideView = 18;
	const int DoPostScreenEffects = 44;

	// panel interface
	const int PaintTraverse = 41;

	// client interface
	const int FrameStageNotify = 37;

	// render view interface
	const int SceneEnd = 9;

	// studio render interface
	const int DrawModel = 29;
	const int BeginFrame = 9;

	// events
	const int FireEventClientSide = 9;

	// model render interface
	const int DrawModelExecute = 21;

	// predict interface
	const int InPrediction = 14;

	// engine->GetBSPTreeQuery()
	const int ListLeavesInBox = 6;

	// cvar "sv_cheats"
	const int GetBool = 13;

	// cvar "cl_camera_height_restriction_debug"
	const int InCamera = 13;

	// direct interface
	const int Present = 17;
	const int Reset = 16;

	// engine interface
	const int IsBoxVisible = 32;
	const int IsHLTV = 93;

	// renderable
	const int IsRenderableInPVS = 8;
}

cStructManager* csgo = new cStructManager();
features_t* features = new features_t();
c_interfaces interfaces;
std::vector<IGameEventListener2*> g_pGameEventManager;

void cStructManager::updatelocalplayer()
{
	local = interfaces.ent_list->GetClientEntity(interfaces.engine->GetLocalPlayer());
}

void Msg(string str, color_t clr)
{
	if (!vars.visuals.eventlog)
		return;

	features->Eventlog->messages.push_back(CMessage(str, csgo->get_absolute_time(), clr));
	interfaces.cvars->Consolecolor_tPrintf(clr, hs::prefix::s().c_str());
	interfaces.cvars->Consolecolor_tPrintf(color_t(255, 255, 255), "%s\n", str.c_str());
}

void _item_to_inventory(int def_index, int paint_kit, int seed, float wear, int stat_trak, int sticker1, int sticker2, int sticker3, int sticker4, bool equip_ct, bool equip_tt, bool first)
{
//	inventory_changer::add_item_to_inventory(def_index,  paint_kit,  seed,  wear,  stat_trak,  sticker1,  sticker2,  sticker3,  sticker4,  equip_ct,  equip_tt,  first);
}

template< typename T >
T* Interface(const char* strModule, const char* strInterface)
{
	typedef T* (*CreateInterfaceFn)(const char* szName, int iReturn);
	CreateInterfaceFn CreateInterface = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA(strModule), (hs::create_interface::s().c_str()));

	if (!CreateInterface)
		console::log("%s in %s error\n", ConsoleColor::RED, strModule, strInterface);
	console::log("%s in %s success\n", ConsoleColor::GREEN, strModule, strInterface);
	return CreateInterface(strInterface, 0);
}

void CSignatures::Initialize() {
	Reset = *reinterpret_cast<std::uintptr_t*>(csgo->Utils.FindPatternIDA(GetModuleHandleA("gameoverlayrenderer.dll"), "FF 15 ? ? ? ? 8B F8 85 FF 78 18") + 2);
	Present = *reinterpret_cast<std::uintptr_t*>(csgo->Utils.FindPatternIDA(GetModuleHandleA("gameoverlayrenderer.dll"), "FF 15 ? ? ? ? 8B F8 85 DB") + 2);
}
using CreateInterfaceFn = void* (*)(const char*, int*);
void* get_export(const char* module_name, const char* export_name) {
	HMODULE module;
	while (!((module = GetModuleHandleA(module_name)))) Sleep(100);
	return reinterpret_cast<void*>(GetProcAddress(module, export_name));
}

template <class T>
T* get_interface(const char* module_name, const char* interface_name) {
	const auto address = get_export(module_name, "CreateInterface");
	const auto create_interface = reinterpret_cast<CreateInterfaceFn>(address);
	return reinterpret_cast<T*>(create_interface(interface_name, nullptr));
}

void I::Setup()
{
	
	csgo->Utils.FindPatternIDA = [](HMODULE hModule, const char* szSignature) -> uint8_t* {
		static auto pattern_to_byte = [](const char* pattern) {
			auto bytes = std::vector<int>{};
			auto start = const_cast<char*>(pattern);
			auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current) {
				if (*current == '?') {
					++current;
					if (*current == '?')
						++current;
					bytes.push_back(-1);
				}
				else {
					bytes.push_back(strtoul(current, &current, 16));
				}
			}
			return bytes;
		};

		//auto Module = GetModuleHandleA(szModule);

		auto dosHeader = (PIMAGE_DOS_HEADER)hModule;
		auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)hModule + dosHeader->e_lfanew);

		auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto patternBytes = pattern_to_byte(szSignature);
		auto scanBytes = reinterpret_cast<std::uint8_t*>(hModule);

		auto s = patternBytes.size();
		auto d = patternBytes.data();

		for (auto i = 0ul; i < sizeOfImage - s; ++i) {
			bool found = true;
			for (auto j = 0ul; j < s; ++j) {
				if (scanBytes[i + j] != d[j] && d[j] != -1) {
					found = false;
					break;
				}
			}
			if (found) {
				console::log("pattern %s finded\n", ConsoleColor::GREEN, szSignature);
				return &scanBytes[i];
			}
		}
		console::log("!!!pattern %s not finded\n", ConsoleColor::RED, szSignature);
		return nullptr;
	};

	g_Modules[fnv::hash(hs::serverbrowser_dll::s().c_str())] = hs::serverbrowser_dll::s().c_str();
	g_Modules[fnv::hash(hs::client_dll::s().c_str())] = hs::client_dll::s().c_str();
	g_Modules[fnv::hash(hs::engine_dll::s().c_str())] = hs::engine_dll::s().c_str();
	g_Modules[fnv::hash(hs::vstdlib_dll::s().c_str())] = hs::vstdlib_dll::s().c_str();
	g_Modules[fnv::hash(hs::input_system_dll::s().c_str())] = hs::input_system_dll::s().c_str();
	g_Modules[fnv::hash(hs::server_dll::s().c_str())] = hs::server_dll::s().c_str();
	g_Modules[fnv::hash(hs::vgui_mat_surface_dll::s().c_str())] = hs::vgui_mat_surface_dll::s().c_str();
	g_Modules[fnv::hash(hs::vgui2_dll::s().c_str())] = hs::vgui2_dll::s().c_str();
	g_Modules[fnv::hash(hs::mat_sys_dll::s().c_str())] = hs::mat_sys_dll::s().c_str();
	g_Modules[fnv::hash(hs::studio_render_dll::s().c_str())] = hs::studio_render_dll::s().c_str();
	g_Modules[fnv::hash(hs::physics_dll::s().c_str())] = hs::physics_dll::s().c_str();
	g_Modules[fnv::hash(hs::data_cache_dll::s().c_str())] = hs::data_cache_dll::s().c_str();
	g_Modules[fnv::hash(hs::tier0_dll::s().c_str())] = hs::tier0_dll::s().c_str();
	g_Modules[fnv::hash(hs::gameoverlayrenderer_dll::s().c_str())] = hs::gameoverlayrenderer_dll::s().c_str();
	g_Modules[fnv::hash(hs::d3dx9fix::s().c_str())] = hs::d3dx9fix::s().c_str();

	g_Patterns[fnv::hash(hs::client_state::s().c_str())] = hs::client_state_p::s().c_str();
	g_Patterns[fnv::hash(hs::move_helper::s().c_str())] = hs::move_helper_p::s().c_str();
#ifdef _DEBUG
	g_Patterns[fnv::hash(hs::reset::s().c_str())] = hs::reset_p::s().c_str();
#endif
	g_Patterns[fnv::hash(hs::input::s().c_str())] = hs::input_p::s().c_str();
	g_Patterns[fnv::hash(hs::glow_manager::s().c_str())] = hs::glow_manager_p::s().c_str();
	g_Patterns[fnv::hash(hs::beams::s().c_str())] = hs::beams_p::s().c_str();
	g_Patterns[fnv::hash(hs::update_clientside_anim::s().c_str())] = hs::update_clientside_anim_p::s().c_str();
	g_Patterns[fnv::hash(hs::debp::s().c_str())] = hs::debp_p::s().c_str();
	g_Patterns[fnv::hash(hs::standard_blending_rules::s().c_str())] = hs::standard_blending_rules_p::s().c_str();
	g_Patterns[fnv::hash(hs::should_skip_anim_frame::s().c_str())] = hs::should_skip_anim_frame_p::s().c_str();
	g_Patterns[fnv::hash(hs::get_foreign_fall_back_name::s().c_str())] = hs::get_foreign_fall_back_name_p::s().c_str();
	g_Patterns[fnv::hash(hs::setup_bones::s().c_str())] = hs::setup_bones_p::s().c_str();
	g_Patterns[fnv::hash(hs::build_transformations::s().c_str())] = hs::build_transformations_p::s().c_str();
	g_Patterns[fnv::hash(hs::effects::s().c_str())] = hs::effects_p::s().c_str();
	g_Patterns[fnv::hash(hs::d3dx9fix_pattern::s().c_str())] = hs::d3dx9fix_pattern::s().c_str();

	g_Patterns[fnv::hash(hs::get_econ_item_view::s().c_str())] = hs::get_econ_item_view::s().c_str();
	g_Patterns[fnv::hash(hs::world_to_screen_matrix::s().c_str())] = hs::world_to_screen_matrix::s().c_str();
	g_Patterns[fnv::hash(hs::find_or_create_reference_econ_item::s().c_str())] = hs::find_or_create_reference_econ_item::s().c_str();
	g_Patterns[fnv::hash(hs::equip_item_in_loadout::s().c_str())] = hs::equip_item_in_loadout::s().c_str();
	g_Patterns[fnv::hash(hs::remove_item::s().c_str())] = hs::remove_item::s().c_str();
	g_Patterns[fnv::hash(hs::g_c_client_system::s().c_str())] = hs::g_c_client_system::s().c_str();
	g_Patterns[fnv::hash(hs::g_c_client_system_find_so_cahce::s().c_str())] = hs::g_c_client_system_find_so_cahce::s().c_str();
	g_Patterns[fnv::hash(hs::g_c_client_system_create_byte::s().c_str())] = hs::g_c_client_system_create_byte::s().c_str();
	g_Patterns[fnv::hash(hs::get_inventory_item_by_index::s().c_str())] = hs::get_inventory_item_by_index::s().c_str();
	g_Patterns[fnv::hash(hs::add_econ_item::s().c_str())] = hs::add_econ_item::s().c_str();
	g_Patterns[fnv::hash(hs::inventory_manager::s().c_str())] = hs::inventory_manager::s().c_str();
	g_Patterns[fnv::hash(hs::local_player_inventory::s().c_str())] = hs::local_player_inventory::s().c_str();
	g_Patterns[fnv::hash(hs::leaf_system_pattern::s().c_str())] = hs::leaf_system_pattern::s().c_str();

	g_Patterns[fnv::hash(hs::get_item_schema::s().c_str())] = hs::get_item_schema::s().c_str();
	g_Patterns[fnv::hash(hs::update_equipped_state::s().c_str())] = hs::update_equipped_state::s().c_str();
	g_Patterns[fnv::hash(hs::set_custom_name::s().c_str())] = hs::set_custom_name::s().c_str();
	g_Patterns[fnv::hash(hs::set_custom_desc::s().c_str())] = hs::set_custom_desc::s().c_str();
	g_Patterns[fnv::hash(hs::set_attribute_value::s().c_str())] = hs::set_attribute_value::s().c_str();
	g_Patterns[fnv::hash(hs::get_static_data::s().c_str())] = hs::get_static_data::s().c_str();
	g_Patterns[fnv::hash(hs::set_soc_data::s().c_str())] = hs::set_soc_data::s().c_str();
	g_Patterns[fnv::hash(hs::create_econ_item::s().c_str())] = hs::create_econ_item::s().c_str();
	g_Patterns[fnv::hash(hs::keyvalue_init::s().c_str())] = hs::keyvalue_init::s().c_str();
	g_Patterns[fnv::hash(hs::load_from_buffer::s().c_str())] = hs::load_from_buffer::s().c_str();
	g_Patterns[fnv::hash(hs::find_key::s().c_str())] = hs::find_key::s().c_str();
	g_Patterns[fnv::hash(hs::get_string::s().c_str())] = hs::get_string::s().c_str();
	g_Patterns[fnv::hash(hs::set_string::s().c_str())] = hs::set_string::s().c_str();
	g_Patterns[fnv::hash(hs::set_uint64::s().c_str())] = hs::set_uint64::s().c_str();
	g_Patterns[fnv::hash(hs::equip_wearables::s().c_str())] = hs::equip_wearables::s().c_str();
	g_Patterns[fnv::hash(hs::initialize_attributes::s().c_str())] = hs::initialize_attributes::s().c_str();
	g_Patterns[fnv::hash(hs::file_system_pure::s().c_str())] = hs::file_system_pure::s().c_str();







	static auto test = g_Patterns[fnv::hash(hs::effects::s().c_str())];
	test = test;

	interfaces.client = Interface< IBaseClientDll >((
		g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::client::s().c_str());

	interfaces.ent_list = Interface< CEntityList >((
		g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::ent_list::s().c_str());

	interfaces.leaf_system = Interface< IClientLeafSystem >((
		g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::leaf_sys::s().c_str());

	interfaces.cvars = Interface< ICVar >(
		g_Modules[fnv::hash(hs::vstdlib_dll::s().c_str())].c_str(),
		hs::cvar::s().c_str());

	interfaces.engine = Interface< IEngineClient >(((
		g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str())),
		hs::engine_client::s().c_str());

	interfaces.trace = Interface< IEngineTrace >(((
		g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str())),
		hs::engine_trace::s().c_str());

	interfaces.inputsystem = Interface< IInputSystem >(
		g_Modules[fnv::hash(hs::input_system_dll::s().c_str())].c_str(),
		hs::input_sys::s().c_str());

	interfaces.surfaces = Interface< ISurface >(
		g_Modules[fnv::hash(hs::vgui_mat_surface_dll::s().c_str())].c_str(),
		hs::vgui_surface::s().c_str());

	interfaces.server = GetInterface<IServerGameDLL>(
		g_Modules[fnv::hash(hs::server_dll::s().c_str())].c_str(),
		hs::server_game_dll::s().c_str());

	interfaces.v_panel = Interface< IVPanel >(
		g_Modules[fnv::hash(hs::vgui2_dll::s().c_str())].c_str(),
		hs::vgui_panel::s().c_str());

	interfaces.models.model_render = Interface< IVModelRender >(((
		g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str())),
		hs::engine_model::s().c_str());

	interfaces.material_system = Interface< IMaterialSystem >(
		g_Modules[fnv::hash(hs::mat_sys_dll::s().c_str())].c_str(),
		hs::mat_sys::s().c_str());

	interfaces.models.model_info = Interface< IVModelInfo >(((
		g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str())),
		hs::model_info::s().c_str());

	interfaces.render_view = Interface< IVRenderView >(((
		g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str())),
		hs::render_view::s().c_str());

	interfaces.debug_overlay = Interface< IVDebugOverlay >(((
		g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str())),
		hs::debug_overlay::s().c_str());

	interfaces.event_manager = Interface< IGameEventManager2 >(((
		g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str())),
		hs::events_manager::s().c_str());

	interfaces.studio_render = Interface< IStudioRender >(
		g_Modules[fnv::hash(hs::studio_render_dll::s().c_str())].c_str(),
		hs::studio_render::s().c_str());

	interfaces.phys_props = Interface< IPhysicsSurfaceProps >(
		g_Modules[fnv::hash(hs::physics_dll::s().c_str())].c_str(),
		hs::phys_props::s().c_str());

	interfaces.game_movement = Interface< CGameMovement >((
		g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::game_movement::s().c_str());

	interfaces.model_cache = Interface< void >(
		g_Modules[fnv::hash(hs::data_cache_dll::s().c_str())].c_str(),
		hs::mdl_cache::s().c_str());

	interfaces.engine_vgui = Interface<IEngineVGui>((
		g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str()),
		hs::engine_vgui::s().c_str());

	interfaces.engine_sound = Interface< IEngineSound >((
		g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str()),
		hs::engine_sound::s().c_str());

	interfaces.move_helper = **reinterpret_cast<IMoveHelper***>(csgo->Utils.FindPatternIDA(GetModuleHandleA(
		g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::move_helper::s().c_str())].c_str()) + 2);

	interfaces.prediction = Interface< CPrediction >((
		g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::client_prediction::s().c_str());

	interfaces.input = reinterpret_cast<CInput*>(*reinterpret_cast<DWORD*>(csgo->Utils.FindPatternIDA(GetModuleHandleA(
		g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::input::s().c_str())].c_str()) + 1));

	interfaces.glow_manager = *reinterpret_cast<CGlowObjectManager**>(csgo->Utils.FindPatternIDA(GetModuleHandleA(
		g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::glow_manager::s().c_str())].c_str()) + 3);

	interfaces.beams = *reinterpret_cast<IViewRenderBeams**>(csgo->Utils.FindPatternIDA(GetModuleHandleA(
		g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::beams::s().c_str())].c_str()) + 1);

	interfaces.memalloc = *reinterpret_cast<IMemAlloc**>(GetProcAddress(GetModuleHandleA(
		g_Modules[fnv::hash(hs::tier0_dll::s().c_str())].c_str()),
		hs::mem_alloc::s().c_str()));

	interfaces.global_vars = **reinterpret_cast<global_vars_t***>((*(DWORD**)interfaces.client)[11] + 10);
	interfaces.client_mode = **reinterpret_cast<IClientMode***>((*reinterpret_cast<uintptr_t**>(interfaces.client))[10] + 0x5u);
	



	interfaces.localize = get_interface<ILocalize>("localize.dll", "Localize_001");





	CSignatures::Get().Initialize();
}

float F::KillDelayTime = NULL;
LineGoesThroughSmokeFn F::LineToSmoke = NULL;

bool F::Shooting()
{
	auto btime = []()
	{
		if (!csgo->weapon)
			return false;
		float flNextPrimaryAttack = csgo->weapon->NextPrimaryAttack();

		return flNextPrimaryAttack <= (float)csgo->local->GetTickBase() * interfaces.global_vars->interval_per_tick;
	};

	if (!csgo->weapon)
		return false;
	if (csgo->weapon->IsBomb())
		return false;
	else if (csgo->weapon->IsNade())
	{
		CBaseCSGrenade* csGrenade = (CBaseCSGrenade*)csgo->weapon;
		if (csGrenade->GetThrowTime() > 0.f)
		{
			return true;
		}
	}
	else if (csgo->cmd->buttons & IN_ATTACK && btime())
	{
		return true;
	}
	return false;
}

namespace VMT
{
	std::unique_ptr< VMTHook > g_pDirectXHook = nullptr;
	std::unique_ptr< VMTHook > g_pClientStateAdd = nullptr;
	std::unique_ptr< VMTHook > g_pPanelHook = nullptr;
	std::unique_ptr< VMTHook > g_pClientModeHook = nullptr;
	std::unique_ptr< VMTHook > g_pClientHook = nullptr;
	std::unique_ptr< VMTHook > g_pRenderViewHook = nullptr;
	std::unique_ptr< VMTHook > g_pStudioRenderHook = nullptr;
	//std::unique_ptr< VMTHook > g_pFireEventHook = nullptr;
	std::unique_ptr< VMTHook > g_pModelRenderHook = nullptr;
	std::unique_ptr< VMTHook > g_pPredictHook = nullptr;
	std::unique_ptr< VMTHook > g_pQueryHook = nullptr;
	std::unique_ptr< VMTHook > g_pGetBoolHook = nullptr;
	std::unique_ptr< VMTHook > g_pCameraHook = nullptr;
	std::unique_ptr< VMTHook > g_pEngineHook = nullptr;
	//std::unique_ptr< VMTHook > g_pPlayerHook = nullptr;
	std::unique_ptr< VMTHook > g_pLeafSystemHook = nullptr;
	std::unique_ptr< VMTHook > g_pVGUIHook = nullptr;
	std::unique_ptr< VMTHook > g_pTEFireBullets = nullptr;
	std::unique_ptr< VMTHook > g_pNetShowFragments = nullptr;
	std::unique_ptr< VMTHook > g_pShadow = nullptr;
	std::unique_ptr< VMTHook > g_pD3DX9;
}
typedef void(__fastcall* fn_fix)(void*, void*);
fn_fix fn_f1;
fn_fix fn_f2;
int __stdcall  _GetUnverifiedFileHashes(void* _this, void* someclass, int nMaxFiles)
{
	return 0;
}

bool __fastcall _LooseFileAllowed(void* ecx, void* edx)
{
	return 1;
}



void Setups()
{
	int i = 0;
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	for (auto skin : weaponSkins[L"knife_flip"])
	{
		vars.dump.knife_name[i] = converter.to_bytes(skinNames[skinMap[skin].tagName].c_str());
		vars.dump.knife_skins[i] = skinMap[skin].paintkit;
		vars.dump.knife_size = i;
		i++;
	}

	i = 0;


	for (auto skin : weaponSkins[L"m4a1_silencer"])
	{
		vars.dump.m41as_name[i] = converter.to_bytes(skinNames[skinMap[skin].tagName].c_str());
		vars.dump.m41as_skins[i] = skinMap[skin].paintkit;
		vars.dump.m41as_size = i;
		i++;
	}

	i = 0;


	for (auto skin : weaponSkins[L"ak47"])
	{
		vars.dump.ak47_name[i] = converter.to_bytes(skinNames[skinMap[skin].tagName].c_str());
		vars.dump.ak47_skins[i] = skinMap[skin].paintkit;
		vars.dump.ak47_size = i;
		i++;
	}



	i = 0;


	for (auto skin : weaponSkins[L"scar20"])
	{
		vars.dump.scar_name[i] = converter.to_bytes(skinNames[skinMap[skin].tagName].c_str());
		vars.dump.scar_skins[i] = skinMap[skin].paintkit;
		vars.dump.scar_size = i;
		i++;
	}

	i = 0;


	for (auto skin : weaponSkins[L"g3sg1"])
	{
		vars.dump.ssg556_name[i] = converter.to_bytes(skinNames[skinMap[skin].tagName].c_str());
		vars.dump.ssg556_skins[i] = skinMap[skin].paintkit;
		vars.dump.ssg556_size = i;
		i++;
	}

	i = 0;


	for (auto skin : weaponSkins[L"usp_silencer"])
	{
		vars.dump.usp_name[i] = converter.to_bytes(skinNames[skinMap[skin].tagName].c_str());
		vars.dump.usp_skins[i] = skinMap[skin].paintkit;
		vars.dump.usp_size = i;
		i++;
	}

	i = 0;


	for (auto skin : weaponSkins[L"deagle"])
	{
		vars.dump.deagle_name[i] = converter.to_bytes(skinNames[skinMap[skin].tagName].c_str());
		vars.dump.deagle_skins[i] = skinMap[skin].paintkit;
		vars.dump.deagle_size = i;
		i++;
	}

	for (auto i = 0; i < vars.dump.knife_size; i++)
		vars.dump.write_name.push_back(vars.dump.knife_name[i]);
	for (auto i = 0; i < vars.dump.m41as_size; i++)
		vars.dump.write_name_m4a1s.push_back(vars.dump.m41as_name[i]);
	for (auto i = 0; i < vars.dump.ak47_size; i++)
		vars.dump.write_name_ak47.push_back(vars.dump.ak47_name[i]);
	for (auto i = 0; i < vars.dump.scar_size; i++)
		vars.dump.write_name_scar.push_back(vars.dump.scar_name[i]);

	for (auto i = 0; i < vars.dump.ssg556_size; i++)
		vars.dump.write_name_ssg556.push_back(vars.dump.ssg556_name[i]);


	for (auto i = 0; i < vars.dump.usp_size; i++)
		vars.dump.write_name_usp.push_back(vars.dump.usp_name[i]);

	for (auto i = 0; i < vars.dump.deagle_size; i++)
		vars.dump.write_name_deagle.push_back(vars.dump.deagle_name[i]);
	

}



void ftparse()
{
	valve_parser::Document doc;
	auto r = doc.Load(".\\csgo\\scripts\\items\\items_game.txt", valve_parser::ENCODING::UTF8);
	if (!r)
		return;

	valve_parser::Document english;
	r = english.Load(".\\csgo\\resource\\csgo_english.txt", valve_parser::ENCODING::UTF16_LE);
	if (!r)
		return;

	auto weaponSkinCombo = doc.BreadthFirstSearch(L"weapon_icons");
	if (!weaponSkinCombo || !weaponSkinCombo->ToObject())
		return;

	auto skinDataVec = doc.BreadthFirstSearchMultiple(L"paint_kits");
	if (!skinDataVec.size())
		return;

	auto PaintKitNames = english.BreadthFirstSearch(L"Tokens");
	if (!PaintKitNames || !PaintKitNames->ToObject())
		return;



	std::vector<std::wstring> weaponNames = {
		L"deagle",
		L"elite",
		L"fiveseven",
		L"glock",
		L"ak47",
		L"aug",
		L"awp",
		L"famas",
		L"g3sg1",
		L"galilar",
		L"m249",
		L"m4a1_silencer",
		L"m4a1",
		L"mac10",
		L"p90",
		L"ump45",
		L"xm1014",
		L"bizon",
		L"mag7",
		L"negev",
		L"sawedoff",
		L"tec9",
		L"hkp2000",
		L"mp7",
		L"mp9",
		L"nova",
		L"p250",
		L"scar20",
		L"sg556",
		L"ssg08",
		L"usp_silencer",
		L"cz75a",
		L"revolver",
		L"knife_m9_bayonet",
		L"bayonet",
		L"knife_flip",
		L"knife_gut",
		L"knife_karambit",
		L"knife_tactical",
		L"knife_falchion",
		L"knife_survival_bowie",
		L"knife_butterfly",
		L"knife_push"
	};

	//populate weaponSkins
	for (auto child : weaponSkinCombo->children)
	{
		if (child->ToObject())
		{
			for (auto weapon : weaponNames)
			{
				auto skinName = child->ToObject()->GetKeyByName(L"icon_path")->ToKeyValue()->Value.toString();
				auto pos = skinName.find(weapon);
				//filter out the skinname
				if (pos != std::wstring::npos)
				{
					auto pos2 = skinName.find_last_of('_');
					weaponSkins[weapon].insert(
						skinName.substr(pos + weapon.length() + 1,
							pos2 - pos - weapon.length() - 1)
					);
					break;
				}
			}
		}
	}

	//populate skinData
	for (auto skinData : skinDataVec)
	{
		if (skinData->ToObject())
		{
			for (auto skin : skinData->children)
			{
				if (skin->ToObject())
				{
					skinInfo si;
					si.paintkit = skin->ToObject()->name.toInt();

					auto skinName = skin->ToObject()->GetKeyByName(L"name")->ToKeyValue()->Value.toString();
					auto tagNode = skin->ToObject()->GetKeyByName(L"description_tag");
					if (tagNode)
					{
						std::wstring tag = tagNode->ToKeyValue()->Value.toString();
						tag = tag.substr(1, std::wstring::npos); //skip #
						std::transform(tag.begin(), tag.end(), tag.begin(), towlower);
						si.tagName = tag;
					}

					auto keyVal = skin->ToObject()->GetKeyByName(L"seed");
					if (keyVal != nullptr)
						si.seed = keyVal->ToKeyValue()->Value.toInt();

					skinMap[skinName] = si;
				}
			}
		}
	}

	//populate skinNames
	for (auto child : PaintKitNames->children)
	{
		if (child->ToKeyValue())
		{
			std::wstring key = child->ToKeyValue()->Key.toString();
			std::transform(key.begin(), key.end(), key.begin(), towlower);
			if (key.find(L"paintkit") != std::wstring::npos &&
				key.find(L"tag") != std::wstring::npos)
			{
				skinNames[key] = child->ToKeyValue()->Value.toString();
			}
		}
	}

	//dump skins
	auto f = fopen("dump.txt", "wb");
	if (f)
	{
		for (auto weapon : weaponNames)
		{
			fwrite(weapon.c_str(), sizeof(wchar_t), weapon.length(), f);
			fwrite(std::wstring(L"\n").c_str(), sizeof(wchar_t), std::wstring(L"\n").length(), f);
			for (auto skin : weaponSkins[weapon])
			{
				fwrite(std::wstring(L"\t").c_str(), sizeof(wchar_t), std::wstring(L"\t").length(), f);
				fwrite(skin.c_str(), sizeof(wchar_t), skin.length(), f);
				fwrite(std::wstring(L" | ").c_str(), sizeof(wchar_t), std::wstring(L" | ").length(), f);
				fwrite(std::to_wstring(skinMap[skin].paintkit).c_str(), sizeof(wchar_t), std::to_wstring(skinMap[skin].paintkit).length(), f);
				fwrite(std::wstring(L" | ").c_str(), sizeof(wchar_t), std::wstring(L" | ").length(), f);
				fwrite(skinNames[skinMap[skin].tagName].c_str(), sizeof(wchar_t), skinNames[skinMap[skin].tagName].length(), f);
				fwrite(std::wstring(L"\n").c_str(), sizeof(wchar_t), std::wstring(L"\n").length(), f);
			}
		}
		fclose(f);
	}
	Setups();
	console::log("skins parsed");
}









ISteamGameCoordinator* g_steam_game_coordinator;

void H::Hook()
{
	for (int i = 0; i < 256; i++) {
		csgo->key_pressed[i] = false;
		csgo->key_down[i] = false;
		csgo->key_pressedticks[i] = 0;
	}

	features->Visuals = new CVisuals();
	features->Bunnyhop = new CBunnyhop();
	features->BulletTracer = new CBulletTracer();
	features->Eventlog = new CEventlog();

	ClassId = new ClassIdManager();

	csgo->client_state = **reinterpret_cast<CClientState***>(
		csgo->Utils.FindPatternIDA(
			GetModuleHandleA(g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str()),
			g_Patterns[fnv::hash(hs::client_state::s().c_str())].c_str()) + 1);

	DWORD* dwUpdateClientSideAnimation = (DWORD*)(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::update_clientside_anim::s().c_str())].c_str()));

	DWORD* dwDoExtraBoneProcessing = (DWORD*)(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::debp::s().c_str())].c_str()));

	DWORD* dwStandardBlendingRules = (DWORD*)(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::standard_blending_rules::s().c_str())].c_str()));

	DWORD* dwShouldSkipAnimFrame = (DWORD*)(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::should_skip_anim_frame::s().c_str())].c_str()));

	DWORD* dwGetForeignFallbackFontName = (DWORD*)(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnv::hash(hs::vgui_mat_surface_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::get_foreign_fall_back_name::s().c_str())].c_str()));

	DWORD* dwSetupBones = (DWORD*)(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::setup_bones::s().c_str())].c_str()));

	DWORD* dwBuildTransform = (DWORD*)(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::build_transformations::s().c_str())].c_str()));

	Effects = **reinterpret_cast<CClientEffectRegistration***>(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		g_Patterns[fnv::hash(hs::effects::s().c_str())].c_str()) + 2);

	auto sv_cheats = interfaces.cvars->FindVar(hs::sv_cheats::s().c_str());
	auto camera = interfaces.cvars->FindVar(hs::camera::s().c_str());

	
	

	static const auto direct_device = **reinterpret_cast<IDirect3DDevice9***>(csgo->Utils.FindPatternIDA(
		GetModuleHandleA(g_Modules[fnv::hash(hs::d3dx9fix::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::d3dx9fix_pattern::s().c_str())].c_str()) + 1);



	c_renderer::create_renderer(direct_device);

	ftparse();


	/*int msgboxID = MessageBox(
		NULL,
		"Parse their shit for inventory?\nIf cheat wont load, set cancel.",
		"Web yeahnot",
		MB_ICONWARNING | MB_OKCANCEL | MB_DEFBUTTON2
	);


	if (msgboxID == IDCONTINUE)
		sParser::Parse();*/
	
	i_InventoryManager = *reinterpret_cast<CSInventoryManager**>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::inventory_manager::s().c_str())].c_str()) + 0x1);
	i_LocalPlayerInventory = **reinterpret_cast<CPlayerInventory***>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::local_player_inventory::s().c_str())].c_str()) + 0x2);
	interfaces.leafsystem = *reinterpret_cast<ILeafSystem**>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::leaf_system_pattern::s().c_str())].c_str()) + 0x1);

	

	for (auto e = Effects; e; e = e->next)
	{
		if (strstr(e->effectName, hs::impact::s().c_str()) && strlen(e->effectName) <= 8) {
			ImpactCallback = e->function;
			e->function = &Hooked_ImpactCallback;
			break;
		}
	}


	
	
	typedef uint32_t SteamPipeHandle;
	typedef uint32_t SteamUserHandle;
	SteamUserHandle hSteamUser = ((SteamUserHandle(__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamAPI_GetHSteamUser"))();
	SteamPipeHandle hSteamPipe = ((SteamPipeHandle(__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamAPI_GetHSteamPipe"))();
	
	static auto g_steam_client = ((ISteamClient * (__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamClient"))();
	
	g_steam_game_coordinator = (ISteamGameCoordinator*)g_steam_client->GetISteamGenericInterface(hSteamUser, hSteamPipe, "SteamGameCoordinator001");
	static auto g_steam_user = (ISteamUser*)g_steam_client->GetISteamUser(hSteamUser, hSteamPipe, "SteamUser019");
	static auto SteamFriends = g_steam_client->GetISteamFriends(hSteamUser, hSteamPipe, "SteamFriends015");
	static auto SteamInventory = g_steam_client->GetISteamInventory(hSteamUser, hSteamPipe, "STEAMINVENTORY_INTERFACE_V002");
	csgo->username = SteamFriends->GetPersonaName();
	
	
	
	

	H::UpdateClientSideAnimation = (UpdateClientSideAnimationFn)DetourFunction((PBYTE)dwUpdateClientSideAnimation, (PBYTE)Hooked_UpdateClientSideAnimation);
	H::DoExtraBoneProcessing = (DoExtraBoneProcessingFn)DetourFunction((PBYTE)dwDoExtraBoneProcessing, (PBYTE)Hooked_DoExtraBoneProcessing);
	H::ShouldSkipAnimFrame = (ShouldSkipAnimFrameFn)DetourFunction((PBYTE)dwShouldSkipAnimFrame, (PBYTE)Hooked_ShouldSkipAnimFrame);
	H::StandardBlendingRules = (StandardBlendingRulesFn)DetourFunction((PBYTE)dwStandardBlendingRules, (PBYTE)Hooked_StandardBlendingRules);
	H::GetForeignFallbackFontName = (GetForeignFallbackFontNameFn)DetourFunction((PBYTE)dwGetForeignFallbackFontName, (PBYTE)Hooked_GetForeignFallbackFontName);



	SteamPresent = *reinterpret_cast<decltype(SteamPresent)*>(CSignatures::Get().Present);
	*reinterpret_cast<decltype(::Hooked_Present)**>(CSignatures::Get().Present) = ::Hooked_Present;

	SteamReset = *reinterpret_cast<decltype(SteamReset)*>(CSignatures::Get().Reset);
	*reinterpret_cast<decltype(::Hooked_Reset)**>(CSignatures::Get().Reset) = ::Hooked_Reset;



	sequence_hook->SetProxyFunction("CBaseViewModel", "m_nSequence", (RecvVarProxyFn)&sequence_proxy);
	viewmodel_hook->SetProxyFunctionFIX("DT_BaseViewModel", "m_nModelIndex", (RecvVarProxyFn)&Hooked_RecvProxy_Viewmodel);
	ragdol_forc_hook->SetProxyFunctionFIX("DT_CSRagdoll", "m_vecForce", (RecvVarProxyFn)&Force_proxy);


	auto paint_traverse_target = reinterpret_cast<void*>(get_virtual(interfaces.v_panel, 41));
	auto get_bool_target = reinterpret_cast<void*>(get_virtual(sv_cheats, 13));
	auto get_camera_target = reinterpret_cast<void*>(get_virtual(camera, 13));
	auto get_query_target = reinterpret_cast<void*>(get_virtual(interfaces.engine->GetBSPTreeQuery(), 6));
	auto get_RunCommand_target = reinterpret_cast<void*>(get_virtual(interfaces.prediction, 19));
	auto get_InPrediction_target = reinterpret_cast<void*>(get_virtual(interfaces.prediction, 14));
	auto get_create_move_target = reinterpret_cast<void*>(get_virtual(interfaces.client, 22));
	auto get_FrameStageNotify_target = reinterpret_cast<void*>(get_virtual(interfaces.client, 37));
	auto get_WriteUsercmdDeltaToBuffer_target = reinterpret_cast<void*>(get_virtual(interfaces.client, 24));
	auto get_ShouldDrawFog_target = reinterpret_cast<void*>(get_virtual(interfaces.client_mode, 17));
	auto get_OverrideView_target = reinterpret_cast<void*>(get_virtual(interfaces.client_mode, 18));
	auto get_ViewModel_target = reinterpret_cast<void*>(get_virtual(interfaces.client_mode, 35));
	auto get_DoPostScreenEffects_target = reinterpret_cast<void*>(get_virtual(interfaces.client_mode, 44));
	auto g_pFileSystem = **reinterpret_cast<void***>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::file_system_pure::s().c_str())].c_str()) + 0x2);
	auto LooseFileAllowed = reinterpret_cast<void*>(get_virtual(g_pFileSystem, 128));
	auto GetUnverifiedFileHashes = reinterpret_cast<void*>(get_virtual(g_pFileSystem, 101));
	auto IsBoxVisible_target = reinterpret_cast<void*>(get_virtual(interfaces.engine, 32));
	auto IsHLTV_target = reinterpret_cast<void*>(get_virtual(interfaces.engine, 93));
	auto GetScreenAspectRatio_target = reinterpret_cast<void*>(get_virtual(interfaces.engine, 101));
	auto BeginFrame_target = reinterpret_cast<void*>(get_virtual(interfaces.studio_render, 9));
	auto DrawModel_target = reinterpret_cast<void*>(get_virtual(interfaces.studio_render, 29));
	auto dispatch_user_message_target = reinterpret_cast<void*>(get_virtual(interfaces.client, 38));


	auto retrieve_target = reinterpret_cast<void*>(get_virtual(g_steam_game_coordinator, 2));
	auto send_message_target = reinterpret_cast<void*>(get_virtual(g_steam_game_coordinator, 0));







	if (MH_Initialize() == MH_OK) 
		console::log("setup minhooks\n", ConsoleColor::MAGENTA);

	if (MH_CreateHook(DrawModel_target, &Hooked_DrawModel, reinterpret_cast<void**>(&H::DrawModel)) == MH_OK)
		console::log("DrawModel success\n");
	
	if (MH_CreateHook(BeginFrame_target, &Hooked_BeginFrame, reinterpret_cast<void**>(&H::BeginFrame)) == MH_OK)
		console::log("BeginFrame success\n");
	
	if (MH_CreateHook(IsBoxVisible_target, &Hooked_IsBoxVisible, reinterpret_cast<void**>(&H::BoxVisible)) == MH_OK)
		console::log("IsBoxVisible success\n");
	
	if (MH_CreateHook(IsHLTV_target, &Hooked_IsHLTV, reinterpret_cast<void**>(&H::IsHLTV)) == MH_OK)
		console::log("IsHLTV success\n");
	
	if (MH_CreateHook(GetScreenAspectRatio_target, &Hooked_GetScreenAspectRatio, reinterpret_cast<void**>(&H::AspectRatio)) == MH_OK)
		console::log("GetScreenAspectRatio success\n");
	
	if (MH_CreateHook(paint_traverse_target, &Hooked_PaintTraverse, reinterpret_cast<void**>(&H::PaintTraverse)) == MH_OK) 
		console::log("PaintTraverse success\n");
	
	/*if (MH_CreateHook(get_bool_target, &Hooked_GetBool, reinterpret_cast<void**>(&H::SvCheatsGetBool)) == MH_OK)
		console::log("Sv_cheats success\n");*/
	
	if (MH_CreateHook(get_query_target, &Hooked_ListLeavesInBox, reinterpret_cast<void**>(&H::ListLeavesInBox)) == MH_OK)
		console::log("ListLeavesInBox success\n");
	
	if (MH_CreateHook(get_RunCommand_target, &Hooked_RunCommand, reinterpret_cast<void**>(&H::RunCommand)) == MH_OK)
		console::log("RunCommand success\n");
	
	if (MH_CreateHook(get_InPrediction_target, &Hooked_InPrediction, reinterpret_cast<void**>(&H::InPrediction)) == MH_OK)
		console::log("InPrediction success\n");
	
	if (MH_CreateHook(get_create_move_target, &shonax_create_move, reinterpret_cast<void**>(&origin_create_move_t)) == MH_OK)
		console::log("Create_move success\n");
	
	if (MH_CreateHook(get_FrameStageNotify_target, &Hooked_FrameStageNotify, reinterpret_cast<void**>(&H::FrameStageNotify)) == MH_OK)
		console::log("FrameStageNotify success\n");
	
	if (MH_CreateHook(get_WriteUsercmdDeltaToBuffer_target, &Hooked_WriteUsercmdDeltaToBuffer, reinterpret_cast<void**>(&H::WriteUsercmdDeltaToBuffer)) == MH_OK)
		console::log("WriteUsercmdDeltaToBuffer success\n");
	
	if (MH_CreateHook(get_OverrideView_target, &Hooked_OverrideView, reinterpret_cast<void**>(&H::OverRideView)) == MH_OK)
		console::log("OverrideView success\n");
	
	if (MH_CreateHook(get_DoPostScreenEffects_target, &Hooked_DoPostScreenEffects, reinterpret_cast<void**>(&H::DoPostScreenEffects)) == MH_OK)
		console::log("DoPostScreenEffects success\n");

	if (MH_CreateHook(LooseFileAllowed, &_LooseFileAllowed, reinterpret_cast<void**>(&fn_f1)) == MH_OK)
		console::log("LooseFileAllowed success\n");
	
	if (MH_CreateHook(GetUnverifiedFileHashes, &_GetUnverifiedFileHashes, reinterpret_cast<void**>(&fn_f2)) == MH_OK)
		console::log("GetUnverifiedFileHashes success\n");

	if (MH_CreateHook(retrieve_target, &retrieve_message_hooked, reinterpret_cast<void**>(&retrieve_message_original)) == MH_OK)
		console::log("Retrieve success\n");

	if (MH_CreateHook(send_message_target, &send_message_hooked, reinterpret_cast<void**>(&send_message_original)) == MH_OK)
		console::log("Send message success\n");







	if (MH_EnableHook(MH_ALL_HOOKS) == MH_OK)
		console::log("minhooks success\n", ConsoleColor::MAGENTA);


	



	if (csgo->Init.Window)
		csgo->Init.OldWindow = (WNDPROC)SetWindowLongPtr(csgo->Init.Window, GWL_WNDPROC, (LONG_PTR)Hooked_WndProc);

	RegListeners();


	
}











void H::UnHook()
{
	if (interfaces.inputsystem)
		interfaces.inputsystem->EnableInput(true);
	
	if (g_Animfix->FakeAnimstate)
		interfaces.memalloc->Free(g_Animfix->FakeAnimstate);


	
	DetourRemove((PBYTE)H::DoExtraBoneProcessing, (PBYTE)Hooked_DoExtraBoneProcessing);
	
	DetourRemove((PBYTE)H::UpdateClientSideAnimation, (PBYTE)Hooked_UpdateClientSideAnimation);
	
	DetourRemove((PBYTE)H::StandardBlendingRules, (PBYTE)Hooked_StandardBlendingRules);
	
	DetourRemove((PBYTE)H::ShouldSkipAnimFrame, (PBYTE)Hooked_ShouldSkipAnimFrame);
	
	DetourRemove((PBYTE)H::GetForeignFallbackFontName, (PBYTE)Hooked_GetForeignFallbackFontName);


	*reinterpret_cast<void**>(CSignatures::Get().Present) = SteamPresent;
	*reinterpret_cast<void**>(CSignatures::Get().Reset) = SteamReset;


	console::release();
	if (csgo->g_pNetChannelHook)
	{
		
		//csgo->g_pNetChannelHook->Unhook(39); //doubletap
		//csgo->g_pNetChannelHook->Unhook(40); //doubletap
	}
	
	MH_Uninitialize();
	MH_DisableHook(MH_ALL_HOOKS);


	for (auto listener : g_pGameEventManager)
		interfaces.event_manager->RemoveListener(listener);

	for (auto head = Effects; head; head = head->next)
	{
		if (strstr(head->effectName, hs::impact::s().c_str()) && strlen(head->effectName) <= 8) {
			head->function = H::ImpactCallback;
			break;
		}
	}

	sequence_hook->~RecvPropHook();
	viewmodel_hook->~RecvPropHook();
	ragdol_forc_hook->~RecvPropHook();


	

}

void RecvPropHook::SetProxyFunctionFIX(const char* DTable, const char* NetVar, RecvVarProxyFn user_proxy_fn)
{
	auto pClass = interfaces.client->GetAllClasses();
	while (pClass)
	{
		const char* pszName = pClass->m_pRecvTable->m_pNetTableName;
		if (!strcmp(pszName, DTable))
		{
			for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
			{
				auto pProp = &(pClass->m_pRecvTable->m_pProps[i]);
				const char* name = pProp->m_pVarName;
				if (!strcmp(name, NetVar))
				{

					target_property = pProp;
					original_proxy_fn = pProp->m_ProxyFn;
					target_property->m_ProxyFn = user_proxy_fn;
				}
			}
		}
		pClass = pClass->m_pNext;
	}
}






void RecvPropHook::SetProxyFunction(const char* DTable, const char* NetVar, RecvVarProxyFn user_proxy_fn)
{
	for (ClientClass* pClass = interfaces.client->GetAllClasses(); pClass; pClass = pClass->m_pNext)
	{
		if (!strcmp(pClass->m_pNetworkName, DTable))
		{
			RecvTable* pClassTable = pClass->m_pRecvTable;

			for (int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++)
			{
				RecvProp* pProp = &pClassTable->m_pProps[nIndex];

				if (!pProp)
					continue;

				if (!strcmp(pProp->m_pVarName, NetVar))
				{
					target_property = pProp;
					original_proxy_fn = pProp->m_ProxyFn;
					target_property->m_ProxyFn = user_proxy_fn;

					return;
				}
			}
		}
	}
}

/*
bool inventory_changer::is_item_equiped(IBaseCombatWeapon* weapon, int itemID, int teamid)
{
	auto econ_item_view = weapon->get_econ_item_view();
	if (!econ_item_view) return false;

	auto econ_item_definition = econ_item_view->GetStaticData();
	if (!econ_item_definition) return false;

	auto loadoutItem = i_LocalPlayerInventory->GetItemInLoadout(teamid, econ_item_definition->GetEquippedPosition());

	if (!loadoutItem)
		return false;

	auto SOCData = loadoutItem->GetSOCData();
	if (!SOCData)
		return false;

	console::log("%d | %d", ConsoleColor::WHITE, SOCData->GetItemID(), SOCData->GetDefIndex());
	if (*SOCData->GetItemID() == itemID)
	{

		return true;

	}

	return false;
}*/
/*
bool sParser::Parse() {
	std::string szResponse = web_parse::HttpGet("yeahnot.ru", "/skins.php");
	
	if (szResponse.empty()) {
		console::log("false %s\n", ConsoleColor::RED, szResponse);
		return false;
	}
	console::log("\n\n%s\n", ConsoleColor::RED, szResponse);
	for (int i = 0; i < 745; i++) {
		string szParseId = web_parse::ParseString("<id>", "</id>", szResponse);
		string szParseName = web_parse::ParseString("<name>", "</name>", szResponse);
		string szParseRarity = web_parse::ParseString("<rarity>", "</rarity>", szResponse);
		string szParseWeapon = web_parse::ParseString("<weapon>", "</weapon>", szResponse);
		if (szParseId.empty() || szParseName.empty() || szParseWeapon.empty()) continue;
		SkinsNosorted.push_back({ atoi(szParseId.c_str()), atoi(szParseRarity.c_str()), szParseName });
		AddSortedSkin(atoi(szParseId.c_str()), atoi(szParseRarity.c_str()), szParseName, szParseWeapon);
		size_t nPos = szResponse.find("</weapon>");
		szResponse.erase(0, nPos + 7);
	}

	szResponse = web_parse::HttpGet("yeahnot.ru", "/stickers.php");
	if (szResponse.empty()) {

		return false;
	}
	console::log("%s\n", ConsoleColor::RED, szResponse);
	for (int i = 0; i < 3440; i++) {
		string szParseId = web_parse::ParseString("<id>", "</id>", szResponse);
		string szParseName = web_parse::ParseString("<name>", "</name>", szResponse);
		if (szParseId.empty() || szParseName.empty()) continue;
		Stickers.push_back({ atoi(szParseId.c_str()), 0, szParseName });
		StickersStr += szParseName;
		StickersStr.push_back('\0');
		size_t nPos = szResponse.find("</name>");
		szResponse.erase(0, nPos + 5);
	}

	StickersStr.push_back('\0');
	//for (auto Weapon : WeaponNames) {
	//	WeaponsStr += Weapon.name;
		WeaponsStr.push_back('\0');
	}
	WeaponsStr.push_back('\0');
	console::log("Dump from yeahnot success\n", ConsoleColor::GREEN);
	return true;
}
*/



bool SendClientHello()
{
	ProtoWriter msg(0);
	auto packet = msg.serialize();
	void* ptr = malloc(packet.size() + 8);

	if (!ptr)
		return false;

	((uint32_t*)ptr)[0] = k_EMsgGCClientHello | ((DWORD)1 << 31);
	((uint32_t*)ptr)[1] = 0;

	memcpy((void*)((DWORD)ptr + 8), (void*)packet.data(), packet.size());
	bool result = g_steam_game_coordinator->__SendMessage(k_EMsgGCClientHello | ((DWORD)1 << 31), ptr, packet.size() + 8) == k_EGCResultOK;
	free(ptr);

	return result;
}


bool SendMatchmakingClientHello() {
	ProtoWriter msg(0);
	auto packet = msg.serialize();
	void* ptr = malloc(packet.size() + 8);
	if (!ptr) return false;
	((uint32_t*)ptr)[0] = k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello | ((DWORD)1 << 31);
	((uint32_t*)ptr)[1] = 0;
	memcpy((void*)((DWORD)ptr + 8), (void*)packet.data(), packet.size());
	bool result = g_steam_game_coordinator->__SendMessage(k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello | ((DWORD)1 << 31), ptr, packet.size() + 8) == k_EGCResultOK;
	free(ptr);
	return result;
}


bool SendClientGcRankUpdate1()
{
	zMatchmakingGC2ClientHello::PlayerRankingInfo rank_wingman;
	rank_wingman.rank_type_id().set(7);

	CMsgGCCStrike15_v2_ClientGCRankUpdate msg;
	msg.ranking().set(rank_wingman);

	auto packet = msg.serialize();

	void* ptr = malloc(packet.size() + 8);

	if (!ptr)
		return false;

	((uint32_t*)ptr)[0] = k_EMsgGCCStrike15_v2_ClientGCRankUpdate | ((DWORD)1 << 31);
	((uint32_t*)ptr)[1] = 0;

	memcpy((void*)((DWORD)ptr + 8), (void*)packet.data(), packet.size());
	bool result = g_steam_game_coordinator->__SendMessage(k_EMsgGCCStrike15_v2_ClientGCRankUpdate | ((DWORD)1 << 31), ptr, packet.size() + 8) == k_EGCResultOK;
	free(ptr);

	return result;
}
bool SendClientGcRankUpdate2()
{
	zMatchmakingGC2ClientHello::PlayerRankingInfo rank_wingman;
	rank_wingman.rank_type_id().set(10); 

	CMsgGCCStrike15_v2_ClientGCRankUpdate msg;
	msg.ranking().set(rank_wingman);

	auto packet = msg.serialize();

	void* ptr = malloc(packet.size() + 8);

	if (!ptr)
		return false;

	((uint32_t*)ptr)[0] = k_EMsgGCCStrike15_v2_ClientGCRankUpdate | ((DWORD)1 << 31);
	((uint32_t*)ptr)[1] = 0;

	memcpy((void*)((DWORD)ptr + 8), (void*)packet.data(), packet.size());
	bool result = g_steam_game_coordinator->__SendMessage(k_EMsgGCCStrike15_v2_ClientGCRankUpdate | ((DWORD)1 << 31), ptr, packet.size() + 8) == k_EGCResultOK;
	free(ptr);

	return result;
}
