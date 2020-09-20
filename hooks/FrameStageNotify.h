#pragma once
#include "Hooks.h"
#include "../features/RageBacktracking.h"
#include "../features/Resolver.h"
#include "../features/AnimationFix.h"
#include "SetupAnimation.h"
#include <intrin.h>
#include <random>
#include "../features/Ragebot.h"

#include "../shonax things/externalparser.h"

#include "../features/keyvalues.h"

#include "../features/econitem.h"
#include "../features/econpaintkitdefinition.h"
#include "../features/playerinventory.h"


#include "../features/inventorychanger.h"


bool __fastcall Hooked_GetBool(void* pConVar, void* edx)
{
	
	if (csgo->DoUnload)
		return H::SvCheatsGetBool(pConVar);

	static auto CAM_THINK = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()),
		hs::retn_camera::s().c_str());
	if (_ReturnAddress() == CAM_THINK)
		return true;

	return H::SvCheatsGetBool(pConVar);
}

static bool enabledtp = false, check = false;

void UpdateCSGOKeyBinds()
{
	if (vars.misc.thirdperson)
	{
		if (!check)
			enabledtp = !enabledtp;
		check = true;
	}
	else
		check = false;


	csgo->SwitchAA = vars.antiaim.inverter->active;
}
inline static CBaseHandle worldmodel_handle;
inline  static IBaseCombatWeapon* worldmodel;






int item_def_knifes()
{
	switch (vars.skins.knife_model)
	{
	case 0:
		return 500;
		break;
	case 1:
		return 514;
		break;
	case 2:
		return 515;
		break;
	case 3:
		return 512;
		break;
	case 4:
		return 505;
		break;
	case 5:
		return 506;
		break;
	case 6:
		return 509;
		break;
	case 7:
		return 507;
		break;
	case 8:
		return 508;
		break;
	case 9:
		return 516;
		break;
	case 10:
		return 520;
		break;
	case 11:
		return 522;
		break;
	case 12:
		return 519;
		break;
	case 13:
		return 523;
		break;
	case 14:
		return 503;
		break;
	case 15:
		return 525;
		break;
	case 16:
		return 521;
		break;
	case 17:
		return 518;
		break;
	case 18:
		return 517;
		break;
	default:
		break;
	}
}





void __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage)
{


	if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	//	inventory_changer::pdu_inventory_items(csgo->local);

	if (interfaces.engine->IsInGame() && interfaces.engine->IsConnected() && vars.skins.enable)
	{
		if (csgo->local && csgo->local->isAlive() && curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		{

			auto weapons = csgo->local->GetWeapons();
			for (auto weapon : weapons)
			{
				if (!weapon) continue;


				if (weapon->GetItemDefinitionIndex() == WEAPON_M4A1S) weapon->FallbackPaintKit() = vars.dump.m41as_skins[vars.skins.weapon_skin[0]];
				if (weapon->GetItemDefinitionIndex() == WEAPON_AK47) weapon->FallbackPaintKit() = vars.dump.ak47_skins[vars.skins.weapon_skin[1]];
				if (weapon->GetItemDefinitionIndex() == WEAPON_SCAR20) weapon->FallbackPaintKit() = vars.dump.scar_skins[vars.skins.weapon_skin[2]];
				if (weapon->GetItemDefinitionIndex() == WEAPON_G3SG1) weapon->FallbackPaintKit() = vars.dump.ssg556_skins[vars.skins.weapon_skin[3]];
				if (weapon->GetItemDefinitionIndex() == WEAPON_USPS) weapon->FallbackPaintKit() = vars.dump.usp_skins[vars.skins.weapon_skin[4]];
				if (weapon->GetItemDefinitionIndex() == WEAPON_DEAGLE) weapon->FallbackPaintKit() = vars.dump.deagle_skins[vars.skins.weapon_skin[5]];



					


				
			/*	if (((IBasePlayer*)weapon)->GetClientClass()->m_ClassID == ClassId->CKnife || ((IBasePlayer*)weapon)->GetClientClass()->m_ClassID == ClassId->CKnifeGG)
				{
					//auto worldmodel = weapon->m_iWorldModelIndex();
					//::worldmodel = reinterpret_cast<IBaseCombatWeapon*>(interfaces.ent_list->GetClientEntityFromHandle(worldmodel));
					weapon->SetItemDefinitionIndex(1) = item_def_knifes();
					const auto weapon_info = sParser::GetWeaponInfo(item_def_knifes());
					if (!weapon_info) continue;
					int varr = interfaces.models.model_info->GetModelIndex(weapon_info->model);
					weapon->ModelIndex() = varr;
					weapon->ViewModelIndex() = varr;

					if (::worldmodel)
						weapon->ModelIndex() = varr + 1;
					weapon->FallbackPaintKit() = vars.dump.knife_skins[vars.skins.knife_skin];
					weapon->GetEntityQuality() = 3;
				}
				weapon->OwnerXuidLow() = 0;
				weapon->OwnerXuidHigh() = 0;
				weapon->FallbackWear() = 0.001f;
				weapon->ItemIDHigh() = 1;
				*/
				

			}





		}
	}
	




	

	if (csgo->client_state != nullptr) {
		if (csgo->g_pNetChannelHook && csgo->g_pNetChannelHook->hooked) {
			if (csgo->client_state->pNetChannel != nullptr) {
				uintptr_t* vtable = *(uintptr_t**)csgo->client_state->pNetChannel;

				if (vtable != csgo->g_pNetChannelHook->shadow_vtable) {
					csgo->g_pNetChannelHook.reset();
				}
			}
			else
				csgo->g_pNetChannelHook.reset();
		}
	}

	if (!interfaces.engine->IsConnected() || !interfaces.engine->IsInGame())
		csgo->mapChanged = true;

	if (!csgo->local || !csgo->local->isAlive()) {
		csgo->ForceOffAA = false;

		if (curStage == FRAME_NET_UPDATE_END) {
			for (int i = 1; i < 65; i++) {
				auto entity = interfaces.ent_list->GetClientEntity(i);
				if (entity != nullptr && entity->IsPlayer() && entity != csgo->local) {
					entity->GetClientSideAnims() = csgo->EnableBones = true;
					entity->UpdateClientSideAnimation();
					entity->GetClientSideAnims() = csgo->EnableBones = false;
				}
			}
			csgo->disable_dt = false;
		}
		if (!csgo->mapChanged)
			return H::FrameStageNotify(curStage);
	}



	if (curStage == FRAME_RENDER_START) {
		features->Visuals->NightMode();
	}

	UpdateCSGOKeyBinds();

	static auto cycle = 0.f;
	static auto anim_time = 0.f;

	if (interfaces.engine->IsConnected() && interfaces.engine->IsInGame())
	{
		static int m_iLastCmdAck = 0;
		static float m_flNextCmdTime = 0.f;

		if (csgo->local && csgo->local->isAlive()) {
			if (csgo->client_state && (m_iLastCmdAck != csgo->client_state->nLastCommandAck || m_flNextCmdTime != csgo->client_state->flNextCmdTime))
			{
				if (csgo->g_flVelMod != csgo->local->GetVelocityModifier())
				{
					*(bool*)((uintptr_t)interfaces.prediction + 0x24) = true;
					csgo->g_flVelMod = csgo->local->GetVelocityModifier();
				}

				m_iLastCmdAck = csgo->client_state->nLastCommandAck;
				m_flNextCmdTime = csgo->client_state->flNextCmdTime;
			}

		}
		if (curStage == FRAME_NET_UPDATE_END)
		{
			static auto r_jiggle_bones = interfaces.cvars->FindVar("r_jiggle_bones");
			r_jiggle_bones->m_fnChangeCallbacks.m_Size = 0;
			if (r_jiggle_bones->GetInt() > 0)
				r_jiggle_bones->SetValue(0);

			for (int i = 1; i < 64; i++) {
				auto entity = interfaces.ent_list->GetClientEntity(i);
				if (csgo->local && entity != nullptr && entity->IsPlayer() && entity != csgo->local && entity->isAlive()) {
					const auto var_map = reinterpret_cast<uintptr_t>(entity) + 36;

					for (auto index = 0; index < *reinterpret_cast<int*>(var_map + 20); index++)
						*reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(var_map) + index * 12) = 0;
				}
			}
			g_Animfix->UpdateFakeState();
			g_Animfix->ApplyLocalPlayer();
			g_Animfix->UpdatePlayers();
		}

		if (curStage == FRAME_RENDER_START)
		{
			if (csgo->local->isAlive()) {
				//if (enabledtp)
				//	*(Vector*)((DWORD)csgo->local + 0x31D8) = csgo->FakeAngle;


				if (csgo->local->isAlive() && enabledtp)
					interfaces.prediction->SetLocalViewAngles(csgo->FakeAngle); // lol





				if (csgo->local->GetFlags() & FL_ONGROUND) {
					csgo->local->GetPlayerAnimState()->m_bOnGround = true;
					csgo->local->GetPlayerAnimState()->m_bInHitGroundAnimation = false;
				}

				if (csgo->local && csgo->local->isAlive() && csgo->local->GetPlayerAnimState())
					csgo->local->SetAbsAngles(Vector(0, csgo->local->GetPlayerAnimState()->m_flGoalFeetYaw, 0));

				interfaces.input->m_fCameraInThirdPerson = false;
				if (vars.visuals.remove & 4)
					csgo->local->GetFlashDuration() = 0.f;

				static float old_simtime = 0.f;
				if (old_simtime != csgo->local->GetSimulationTime())
				{
					csgo->all_data[0].sim_time = old_simtime;
					csgo->all_data[0].interp_time = 0.f;
					csgo->all_data[0].origin = csgo->local->GetAbsOriginVec();
					old_simtime = csgo->local->GetSimulationTime();
				}

				csgo->all_data[1].sim_time = csgo->local->GetSimulationTime();

				if (!csgo->send_packet) {
					csgo->all_data[1].interp_time = 0.f;
					csgo->all_data[1].origin = csgo->local->GetAbsOriginVec();
				}
				auto animstate = csgo->local->GetPlayerAnimState();
				if (csgo->animstate && animstate)
					csgo->desync_angle = clamp(fabsf(Math::NormalizeYaw(csgo->animstate->m_flGoalFeetYaw - animstate->m_flGoalFeetYaw)), 0.f, 58.f);
			}
			features->BulletTracer->Proceed();
		}

		csgo->disable_dt = false;
		csgo->ForceOffAA = false;
	}
	else {
		csgo->disable_dt = false;
		csgo->mapChanged = true;
		csgo->dt_charged = false;
		csgo->skip_ticks = 0;
	}

	H::FrameStageNotify(curStage);
}

bool __fastcall Hooked_ShouldDrawFog(void* ecx, void* edx) {
	return !(vars.visuals.remove & 64);
}

bool __fastcall Hooked_ShouldDrawShadow(void*, uint32_t) {
	return !(vars.visuals.remove & 128);
}



namespace DetailFnv
{
	template <typename Type, Type OffsetBasis, Type Prime>
	struct size_dependant_data
	{
		using type = Type;
		constexpr static auto k_offset_basis = OffsetBasis;
		constexpr static auto k_prime = Prime;
	};

	template <size_t Bits>
	struct size_selector;

	template <>
	struct size_selector<32>
	{
		using type = size_dependant_data<std::uint32_t, 0x811c9dc5ul, 16777619ul>;
	};

	template <>
	struct size_selector<64>
	{
		using type = size_dependant_data<std::uint64_t, 0xcbf29ce484222325ull, 1099511628211ull>;
	};

	template <std::size_t Size>
	class fnv_hash
	{
	private:
		using data_t = typename size_selector<Size>::type;

	public:
		using hash = typename data_t::type;

	private:
		constexpr static auto k_offset_basis = data_t::k_offset_basis;
		constexpr static auto k_prime = data_t::k_prime;

	public:
		template <std::size_t N>
		static __forceinline constexpr auto hash_constexpr(const char(&str)[N], const std::size_t size = N) -> hash
		{
			return static_cast<hash>(1ull * (size == 1
				? (k_offset_basis ^ str[0])
				: (hash_constexpr(str, size - 1) ^ str[size - 1])) * k_prime);
		}

		static auto __forceinline hash_runtime(const char* str) -> hash
		{
			auto result = k_offset_basis;
			do
			{
				result ^= *str++;
				result *= k_prime;
			} while (*(str - 1) != '\0');

			return result;
		}
	};
}

using new_fnv = ::DetailFnv::fnv_hash<sizeof(void*) * 8>;

#define FNV(str) (std::integral_constant<new_fnv::hash, new_fnv::hash_constexpr(str)>::value)



static int get_new_animation(const new_fnv::hash model, const int sequence) {
	enum ESequence {
		SEQUENCE_DEFAULT_DRAW = 0,
		SEQUENCE_DEFAULT_IDLE1 = 1,
		SEQUENCE_DEFAULT_IDLE2 = 2,
		SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
		SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
		SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
		SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
		SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
		SEQUENCE_DEFAULT_LOOKAT01 = 12,
	};
	switch (model) {
	case FNV("models/weapons/v_knife_butterfly.mdl"): {
		switch (sequence) {
		case SEQUENCE_DEFAULT_DRAW: return Math::RandomInt(0, 1);
		case SEQUENCE_DEFAULT_LOOKAT01: return Math::RandomInt(13, 15);
		default: return sequence + 1;
		}
	}
	case FNV("models/weapons/v_knife_falchion_advanced.mdl"): {
		switch (sequence) {
		case SEQUENCE_DEFAULT_IDLE2: return 1;
		case SEQUENCE_DEFAULT_HEAVY_MISS1: return Math::RandomInt(8, 9);
		case SEQUENCE_DEFAULT_LOOKAT01: return Math::RandomInt(12, 13);
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1: return sequence;
		default: return sequence - 1;
		}
	}
	case FNV("models/weapons/v_knife_push.mdl"): {
		switch (sequence) {
		case SEQUENCE_DEFAULT_IDLE2: return 1;
		case SEQUENCE_DEFAULT_LIGHT_MISS1:
		case SEQUENCE_DEFAULT_LIGHT_MISS2: return Math::RandomInt(2, 6);
		case SEQUENCE_DEFAULT_HEAVY_MISS1: return Math::RandomInt(11, 12);
		case SEQUENCE_DEFAULT_HEAVY_HIT1:
		case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
		case SEQUENCE_DEFAULT_LOOKAT01: return sequence + 3;
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1: return sequence;
		default: return sequence + 2;
		}
	}
	case FNV("models/weapons/v_knife_survival_bowie.mdl"): {
		switch (sequence) {
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1: return sequence;
		case SEQUENCE_DEFAULT_IDLE2: return 1;
		default: return sequence - 1;
		}
	}
	case FNV("models/weapons/v_knife_ursus.mdl"): {
		switch (sequence) {
		case SEQUENCE_DEFAULT_DRAW: return Math::RandomInt(0, 1);
		case SEQUENCE_DEFAULT_LOOKAT01: return Math::RandomInt(13, 14);
		default: return sequence + 1;
		}
	}
	case FNV("models/weapons/v_knife_stiletto.mdl"): {
		switch (sequence) {
		case SEQUENCE_DEFAULT_LOOKAT01: return Math::RandomInt(12, 13);
		}
	}
	case FNV("models/weapons/v_knife_widowmaker.mdl"): {
		switch (sequence) {
		case SEQUENCE_DEFAULT_LOOKAT01: return Math::RandomInt(14, 15);
		}
	}
	default: return sequence;
	}
}




void sequence_remap(CRecvProxyData* data, IBaseViewModel* view_model, IBasePlayer* local) {

	if (!vars.skins.enable) return;
	const auto owner = interfaces.ent_list->GetClientEntityFromHandle(view_model->m_hOwner());
	if (local && owner && owner == local) {
		if (local->is_controlling_bot()) return;
		auto weapon = local->GetWeapon();
		if (!weapon) return;
		//const auto weapon_info = sParser::GetWeaponInfo(weapon->GetItemDefinitionIndex());
		//if (!weapon_info) return;
		//Replace anim
		auto& m_sequence = data->m_Value.m_Int;
		//m_sequence = get_new_animation(new_fnv::hash_runtime(weapon_info->model), m_sequence);
	}
}

RecvPropHook* sequence_hook = new RecvPropHook();
void _cdecl sequence_proxy(const CRecvProxyData* data_const, void* p_struct, void* p_out) {
	const auto data = const_cast<CRecvProxyData*>(data_const);
	const auto view_model = static_cast<IBaseViewModel*>(p_struct);
//	if (view_model && data && csgo->local) sequence_remap(data, view_model, csgo->local);
	sequence_hook->GetOriginalFunction()(data, p_struct, p_out);
}








RecvPropHook* ragdol_forc_hook = new RecvPropHook();

class Ragdoll : public IBasePlayer {
public:
	__forceinline IBasePlayer* GetPlayer() {
		return  reinterpret_cast<IBasePlayer*>(interfaces.ent_list->GetClientEntityFromHandle(m_hPlayer()));
	}
	//m_hPlayer: 0x2984
	//m_hPlayer: 0x9E4
	//m_hPlayer: 0x29F0
	__forceinline uint32_t& m_hPlayer() {
		return *(uint32_t*)((std::uintptr_t)this + 0x29F0);
	}
};


void Force_proxy(CRecvProxyData* data, void* ptr, void* out) {
	// convert to ragdoll.
	Ragdoll* ragdoll = reinterpret_cast<Ragdoll*>(ptr);
	// get ragdoll owner.
	IBasePlayer* player = ragdoll->GetPlayer();

	// we only want this happening to noobs we kill.
	if (/*g_cfg[XOR("misc_ragdoll_force")].get<bool>() &&*/ csgo->local && player && csgo->local != player) {
		// get m_vecForce.
		Vector vel = { data->m_Value.m_Vector[0], data->m_Value.m_Vector[1], data->m_Value.m_Vector[2] };

		// give some speed to all directions.
		vel *= 1000.f;

		// boost z up a bit.
		if (vel.z <= 1.f)
			vel.z = 2.f;

		vel.z *= 2.f;
#undef max
		// don't want crazy values for this... probably unlikely though?
		std::clamp(vel.x, std::numeric_limits< float >::lowest(), std::numeric_limits< float >::max());
		std::clamp(vel.y, std::numeric_limits< float >::lowest(), std::numeric_limits< float >::max());
		std::clamp(vel.z, std::numeric_limits< float >::lowest(), std::numeric_limits< float >::max());


		// set new velocity.
		data->m_Value.m_Vector[0] = vel.x;
		data->m_Value.m_Vector[1] = vel.y;
		data->m_Value.m_Vector[2] = vel.z;
	}

	if (ragdol_forc_hook)
		ragdol_forc_hook->GetOriginalFunction()(data, ptr, out);
}






RecvPropHook* viewmodel_hook = new RecvPropHook();
void Hooked_RecvProxy_Viewmodel(CRecvProxyData* pData, void* pStruct, void* pOut)
{
	// Get the knife view model id's
	int default_t = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_default_t.mdl");
	int default_ct = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
	int iBayonet = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
	int iButterfly = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
	int iFlip = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_flip.mdl");
	int iGut = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_gut.mdl");
	int iKarambit = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_karam.mdl");
	int iM9Bayonet = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
	int iHuntsman = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_tactical.mdl");
	int iFalchion = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
	int iDagger = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_push.mdl");
	int iBowie = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");
	int iGunGame = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_gg.mdl");
	int Navaja = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_gypsy_jackknife.mdl");
	int Stiletto = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_stiletto.mdl");
	int Ursus = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_ursus.mdl");
	int Talon = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_widowmaker.mdl");
	int d1 = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_css.mdl");
	int d2 = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_skeleton.mdl");
	int d3 = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_outdoor.mdl");
	int d4 = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_canis.mdl");
	int d5 = interfaces.models.model_info->GetModelIndex("models/weapons/v_knife_cord.mdl");


	const auto local = csgo->local;


	if (local && vars.skins.enable)
	{
		if (local->isAlive() && (
			pData->m_Value.m_Int == default_t ||
			pData->m_Value.m_Int == default_ct ||
			pData->m_Value.m_Int == iBayonet ||
			pData->m_Value.m_Int == iFlip ||
			pData->m_Value.m_Int == iGunGame ||
			pData->m_Value.m_Int == iGut ||
			pData->m_Value.m_Int == iKarambit ||
			pData->m_Value.m_Int == iM9Bayonet ||
			pData->m_Value.m_Int == iHuntsman ||
			pData->m_Value.m_Int == iBowie ||
			pData->m_Value.m_Int == iButterfly ||
			pData->m_Value.m_Int == iFalchion ||
			pData->m_Value.m_Int == iDagger ||
			pData->m_Value.m_Int == Navaja ||
			pData->m_Value.m_Int == Stiletto ||
			pData->m_Value.m_Int == Ursus ||
			pData->m_Value.m_Int == Talon ||
				   
			pData->m_Value.m_Int == d1 ||
			pData->m_Value.m_Int == d2 ||
			pData->m_Value.m_Int == d3 ||
			pData->m_Value.m_Int == d4 ||
			pData->m_Value.m_Int == d5))
		{

			
			
			//const auto weapon_info = sParser::GetWeaponInfo(item_def_knifes());
		//	if (weapon_info)
			//	pData->m_Value.m_Int = interfaces.models.model_info->GetModelIndex(weapon_info->model);
			


			
		}
	}

	viewmodel_hook->GetOriginalFunction()(pData, pStruct, pOut);
}



#include "../features/pbwrap.hpp"


using namespace pbwrap;



#include "protoparse.h"
#include "protomessages.h"
#include "../shonax things/parser/dyn_shonax.h"
#include "../shonax things/parser/ptr.h"

std::string proto_r_profile(void* pub_dest, uint32_t* p_cub_msg_size) {
	MatchmakingGC2ClientHello msg((void*)((DWORD)pub_dest + 8), *p_cub_msg_size - 8);
	auto commendation_new = msg.get_commendation<PlayerCommendationInfo>();
	if (vars.profile.commendation_friendly > 0) commendation_new.replace_cmd_friendly(vars.profile.commendation_friendly);
	if (vars.profile.commendation_teaching > 0) commendation_new.replace_cmd_teaching(vars.profile.commendation_teaching);
	if (vars.profile.commendation_leader > 0) commendation_new.replace_cmd_leader(vars.profile.commendation_leader);
	msg.replace_commendation(commendation_new.serialize());
	auto ranking_new = msg.get_ranking<PlayerRankingInfo>();
	if (vars.profile.rank > 0) ranking_new.replace_rank_id(vars.profile.rank);
	if (vars.profile.wins > 0) ranking_new.replace_wins(vars.profile.wins);
	msg.replace_ranking(ranking_new.serialize());
	if (vars.profile.level > 0) msg.replace_player_level(vars.profile.level);
	return msg.serialize();
}



#define k_EMsgGCCStrike15_v2_MatchmakingGC2ClientReserve 9107
#define k_EMsgGCClientWelcome 4004
#define k_EMsgGCClientHello 4006
#define k_EMsgGCAdjustItemEquippedState 1059
#define k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello 9109
#define k_EMsgGCCStrike15_v2_MatchmakingGC2ClientHello 9110
#define k_EMsgGCCStrike15_v2_ClientGCRankUpdate  9194




using eFn = int __fastcall(void*, void*, uint32_t*, void*, uint32_t, uint32_t*);
static eFn* retrieve_message_original;





void WritePacket(string packet, void* thisPtr, void* oldEBP, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize) {
	#define CAST(cast, address, add) reinterpret_cast<cast>((uint32_t)address + (uint32_t)add)
	if ((uint32_t)packet.size() <= cubDest - 8) {
		memcpy((void*)((DWORD)pubDest + 8), (void*)packet.data(), packet.size());
		*pcubMsgSize = packet.size() + 8;
	}
	else if (interfaces.memalloc) {
		auto memPtr = *CAST(void**, thisPtr, 0x14);
		auto memPtrSize = *CAST(uint32_t*, thisPtr, 0x18);
		auto newSize = (memPtrSize - cubDest) + packet.size() + 8;
		auto memory = interfaces.memalloc->Realloc(memPtr, newSize + 4);
		*CAST(void**, thisPtr, 0x14) = memory;
		*CAST(uint32_t*, thisPtr, 0x18) = newSize;
		*CAST(void**, oldEBP, -0x14) = memory;
		memcpy(CAST(void*, memory, 24), (void*)packet.data(), packet.size());
		*pcubMsgSize = packet.size() + 8;
	}
}




struct zMatchmakingGC2ClientHello : pbmsg<20> {
	struct PlayerRankingInfo : pbmsg<6> {
		PBMSG_CTOR;
		PBFIELD(1, types::Uint32, account_id);
		PBFIELD(2, types::Uint32, rank_id);
		PBFIELD(3, types::Uint32, wins);
		PBFIELD(6, types::Uint32, rank_type_id);
	};
	struct PlayerCommendationInfo : pbmsg<4> {
		PBMSG_CTOR;
		PBFIELD(1, types::Uint32, cmd_friendly);
		PBFIELD(2, types::Uint32, cmd_teaching);
		PBFIELD(4, types::Uint32, cmd_leader);
	};

	PBMSG_CTOR;
	PBFIELD(7, PlayerRankingInfo, ranking);
	PBFIELD(8, PlayerCommendationInfo, commendation);
	PBFIELD(17, types::Int32, player_level);
};





struct CMsgGCCStrike15_v2_ClientGCRankUpdate : pbmsg<1> {
	PBMSG_CTOR;
	PBFIELD(1, zMatchmakingGC2ClientHello::PlayerRankingInfo, ranking);
};



int __fastcall retrieve_message_hooked(void* thisptr, void*, uint32_t* pun_msg_type, void* pub_dest, uint32_t cub_dest, uint32_t* p_cub_msg_size) {
	auto status = retrieve_message_original(thisptr, nullptr, pun_msg_type, pub_dest, cub_dest, p_cub_msg_size);
	uint32_t message_type = *pun_msg_type & 0x7FFFFFFF;
	if (status == 0) {
		void* this_ptr = nullptr;
		__asm mov this_ptr, ebx;
		auto old_ebp = *reinterpret_cast<void**>((uint32_t)_AddressOfReturnAddress() - 4);
		if (message_type == k_EMsgGCCStrike15_v2_MatchmakingGC2ClientHello) {
			auto packet = proto_r_profile(pub_dest, p_cub_msg_size);
			WritePacket(packet, this_ptr, old_ebp, pub_dest, cub_dest, p_cub_msg_size);
		}
		else if (message_type == k_EMsgGCCStrike15_v2_ClientGCRankUpdate)
		{
			CMsgGCCStrike15_v2_ClientGCRankUpdate msg((void*)((DWORD)pub_dest + 8), *p_cub_msg_size - 8);

			auto ranking = msg.ranking().get();

			if (ranking.rank_type_id().get() == 7)
			{
				ranking.rank_id().set(vars.profile.r_rank);
				ranking.wins().set(vars.profile.r_wins);
				msg.ranking().set(ranking);
				auto packet = msg.serialize();
				WritePacket(packet, this_ptr, old_ebp, pub_dest, cub_dest, p_cub_msg_size);
			}
			else
				if (ranking.rank_type_id().get() == 10) {
					ranking.rank_id().set(vars.profile.t_rank);
					ranking.wins().set(vars.profile.t_wins);
					msg.ranking().set(ranking);
					auto packet = msg.serialize();
					WritePacket(packet, this_ptr, old_ebp, pub_dest, cub_dest, p_cub_msg_size);
				}
		}
	}
	return status;
} 





using sFn = int __fastcall(void*, void*, uint32_t, const void*, uint32_t);
static sFn* send_message_original;


int __fastcall send_message_hooked(void* thisptr, void*, uint32_t un_msg_type, const void* pub_data, uint32_t cub_data) {
	auto send_message = true;
	uint32_t message_type = un_msg_type & 0x7FFFFFFF;
	if (!send_message) return 0;
	send_message_original(thisptr, nullptr, un_msg_type, const_cast<void*>(pub_data), cub_data);
} 



