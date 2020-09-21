#include "../hooks/Hooks.h"
#include "AntiAims.h"
#include "Ragebot.h"
#include "Resolver.h"

bool CanDT() {
	int idx = csgo->weapon->GetItemDefinitionIndex();
	return csgo->local->isAlive() && csgo->weapon->DTable()
		&& csgo->client_state->iChokedCommands <= 1 
		&& idx != WEAPON_REVOLVER
		&& idx != WEAPON_ZEUSX27
		&& vars.ragebot.double_tap->active && !csgo->fake_duck;
}

bool CanHS() {
	return
		false;
}

void CMAntiAim::Fakelag(bool& send_packet)
{
	if (!vars.antiaim.enable)
		return;

	bool dt = CanDT();
	bool hs = CanHS();

	bool exp = dt || hs;

	if (csgo->fake_duck && csgo->local->GetFlags() & FL_ONGROUND && !(csgo->cmd->buttons & IN_JUMP))
	{
		if (csgo->local->GetFlags() & FL_ONGROUND)
			return;
	}

	if (CanHS()
		|| interfaces.engine->IsVoiceRecording()) {
		csgo->max_fakelag_choke = 1;
		return;
	}

	if (dt && did_shot)
		return;

	if ((csgo->cmd->buttons & IN_ATTACK) && !vars.antiaim.fakelag_onshot) {
		send_packet = true;
		csgo->max_fakelag_choke = exp ? 2 : 10;
		return;
	}

	if (exp || csgo->local->GetVelocity().Length2D() < 10.f)
	{
		send_packet = exp ? csgo->client_state->iChokedCommands >= 1 : csgo->client_state->iChokedCommands >= 1;

		csgo->max_fakelag_choke = 1/*exp ? 1 : vars.antiaim.break_lby ? 2 : 1*/;
		return;
	}

	auto animstate = csgo->local->GetPlayerAnimState();
	if (!animstate)
		return;

	int tick_to_choke = 1;
	csgo->max_fakelag_choke = vars.misc.antiuntrusted ? 6 : 14;

	static Vector oldOrigin;

	if (!(csgo->local->GetFlags() & FL_ONGROUND))
	{
		csgo->canDrawLC = true;
	}
	else {
		csgo->canDrawLC = false;
		csgo->canBreakLC = false;
	}

	if (vars.antiaim.fakelag < 0)
		tick_to_choke = 2;
	else
	{
		switch (vars.antiaim.fakelag)
		{
		case 0:
			tick_to_choke = 1;
			break;
		case 1:
			tick_to_choke = vars.antiaim.fakelagfactor;
			break;
		case 2:
		{
			int factor = vars.antiaim.fakelagvariance;
			if (factor == 0)
				factor = 15;
			else if (factor > 100)
				factor = 100;

			if (csgo->cmd->command_number % factor < vars.antiaim.fakelagfactor)
				tick_to_choke = min(vars.antiaim.fakelagfactor, csgo->max_fakelag_choke);
			else
				tick_to_choke = 1;
		}
		break;
		}
	}

	if (tick_to_choke < 1)
		tick_to_choke = 1;

	if (tick_to_choke > csgo->max_fakelag_choke)
		tick_to_choke = csgo->max_fakelag_choke;

	send_packet = csgo->client_state->iChokedCommands >= tick_to_choke;

	static Vector sent_origin = Vector();

	if (csgo->canDrawLC) {
		if (send_packet)
			sent_origin = csgo->local->GetAbsOrigin();

		if ((sent_origin - oldOrigin).LengthSqr() > 4096.f) {
			csgo->canBreakLC = true;
		}
		else
			csgo->canBreakLC = false;

		if (send_packet)
			oldOrigin = csgo->local->GetAbsOrigin();
	}
}

void CMAntiAim::Freestanding()
{
	if (!csgo->local->isAlive())
		return;

	if (!csgo->weapon->IsGun())
		return;


	for (int i = 1; i < interfaces.engine->GetMaxClients(); ++i)
	{
		auto player = interfaces.ent_list->GetClientEntity(i);

		if (!player || !player->isAlive() || player->IsDormant() || player->GetTeam() == csgo->local->GetTeam())
			continue;

		bool Autowalled = false, HitSide1 = false, HitSide2 = false;
		auto idx =

			player->GetIndex();
		float angToLocal = Math::CalculateAngle(csgo->local->GetOrigin(),

			player->GetOrigin()).y;
		Vector ViewPoint = csgo->local->GetOrigin() + Vector(0, 0, 90);
		Vector2D Side1 = { (45 * sin(DEG2RAD(angToLocal))),(45 * cos(DEG2RAD(angToLocal))) };
		Vector2D Side2 = { (45 * sin(DEG2RAD(angToLocal + 180))) ,(45 * cos(DEG2RAD(angToLocal + 180))) };

		Vector2D Side3 = { (58 * sin(DEG2RAD(angToLocal))),(58 * cos(DEG2RAD(angToLocal))) };
		Vector2D Side4 = { (58 * sin(DEG2RAD(angToLocal + 180))) ,(58 * cos(DEG2RAD(angToLocal + 180))) };

		Vector Origin = player->GetOrigin();

		Vector2D OriginLeftRight[] = { Vector2D(Side1.x, Side1.y), Vector2D(Side2.x, Side2.y) };

		Vector2D OriginLeftRightLocal[] = { Vector2D(Side3.x, Side3.y), Vector2D(Side4.x, Side4.y) };

		for (int side = 0; side < 2; side++)
		{
			Vector OriginAutowall = { Origin.x + OriginLeftRight[side].x, Origin.y - OriginLeftRight[side].y , Origin.z + 90 };
			Vector ViewPointAutowall = { ViewPoint.x + OriginLeftRightLocal[side].x, ViewPoint.y - OriginLeftRightLocal[side].y , ViewPoint.z };

			if (g_AutoWall.CanHitFloatingPoint(OriginAutowall, ViewPoint))
			{
				if (side == 0)
				{
					HitSide1 = true;
					FreestandSide2[idx] = -1;
				}
				else if (side == 1)
				{
					HitSide2 = true;
					FreestandSide2[idx] = 1;
				}

				Autowalled = true;
			}
			else
			{
				for (int sidealternative = 0; sidealternative < 2; sidealternative++)
				{
					Vector ViewPointAutowallalternative = { Origin.x + OriginLeftRight[sidealternative].x, Origin.y - OriginLeftRight[sidealternative].y , Origin.z + 90 };

					if (g_AutoWall.CanHitFloatingPoint(ViewPointAutowallalternative, ViewPointAutowall))
					{
						if (sidealternative == 0)
						{
							HitSide1 = true;
							FreestandSide2[idx] = -1;
							//FreestandAngle[pPlayerEntity->EntIndex()] = 90;
						}
						else if (sidealternative == 1)
						{
							HitSide2 = true;
							FreestandSide2[idx] = 1;
							//FreestandAngle[pPlayerEntity->EntIndex()] = -90;
						}

						Autowalled = true;
					}
				}
			}
		}
	}
}

void CMAntiAim::Pitch()
{
	switch (vars.antiaim.pitch % 4) {
	case 0: csgo->cmd->viewangles.x = 90; break;
	case 1:	csgo->cmd->viewangles.x = 0; break;
	case 2:	csgo->cmd->viewangles.x = -90; break;
	case 3:	csgo->cmd->viewangles.x = -180; break;
	}
}
void CMAntiAim::Sidemove() {
	if (!csgo->should_sidemove)
		return;

	float sideAmount = 2 * ((csgo->cmd->buttons & IN_DUCK || csgo->cmd->buttons & IN_WALK) ? 3.f : 0.505f); 
	if (csgo->local->GetVelocity().Length2D() <= 0.f || std::fabs(csgo->local->GetVelocity().z) <= 100.f)
		csgo->cmd->sidemove += csgo->cmd->command_number % 2 ? sideAmount : -sideAmount;
}

float GetCurtime() {
	if (!csgo->local)
		return 0;
	int g_tick = 0;
	CUserCmd* g_pLastCmd = nullptr;
	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) {
		g_tick = csgo->local->GetTickBase();
	}
	else {
		++g_tick;
	}
	g_pLastCmd = csgo->cmd;
	float curtime = g_tick * interfaces.global_vars->interval_per_tick;
	return curtime;
}
float CMAntiAim::corrected_tickbase()
{
	CUserCmd* last_ucmd = nullptr;
	int corrected_tickbase = 0;

	corrected_tickbase = (!last_ucmd || last_ucmd->hasbeenpredicted) ? (float)csgo->local->GetTickBase() : corrected_tickbase++;
	last_ucmd = csgo->cmd;
	float corrected_curtime = corrected_tickbase * interfaces.global_vars->interval_per_tick;
	return corrected_curtime;

};

void CMAntiAim::predict_lby_update(float sampletime, CUserCmd* ucmd, bool& sendpacket)
{
	csgo->InLbyUpdate = false;
	static float next_lby_update_time = 0;
	auto local = csgo->local;

	if (!(local->GetFlags() & 1))
		return;

	if (local->GetVelocity().Length2D() > 0.1f)
		next_lby_update_time = corrected_tickbase() + 0.22f;
	else if (next_lby_update_time - corrected_tickbase() <= 0.0f) {
		next_lby_update_time = corrected_tickbase() + 1.1f;
		csgo->InLbyUpdate = true;
		sendpacket = false;
	}
	else if (next_lby_update_time - corrected_tickbase() <= 1 * interfaces.global_vars->interval_per_tick)
		sendpacket = true;
}







void CMAntiAim::Yaw(bool& send_packet)
{

	float inverter = vars.antiaim.inverter->active ? 0 : 1;



	static float s_add = 0.f;
	static bool b_switch = false;
	int side = csgo->SwitchAA ? 1 : -1;
	if (csgo->desync_angle < (float)vars.antiaim.desync_amount / 100.f)
		side *= 1.f;
	else if (vars.antiaim.slowwalk->active) {
		if (csgo->desync_angle < 58.f) {
			if (b_switch)
				s_add += 2.f;
			else
				s_add -= 2.f;
		}
		if (s_add > 30.f) {
			b_switch = !b_switch;
			s_add = 0.f;
		}
	}
	else
		b_switch = csgo->SwitchAA;


	

	if (vars.antiaim.aa_override.enable)
	{
		static bool left, right, back;
		if (vars.antiaim.aa_override.left->active)
		{
			left = true;
			right = false;
			back = false;
		}
		else if (vars.antiaim.aa_override.right->active)
		{
			left = false;
			right = true;
			back = false;
		}
		else if (vars.antiaim.aa_override.back->active)
		{
			left = false;
			right = false;
			back = true;
		}

		if (left)
			csgo->cmd->viewangles.y -= 90;
		if (right)
			csgo->cmd->viewangles.y += 90;
	}

	csgo->cmd->viewangles.y -= 180.f;
	static auto sw = false;
	switch (vars.antiaim.desync_type)
	{
	case 0:
		csgo->should_sidemove = true;
		if (!send_packet)
			csgo->cmd->viewangles.y -= 58.f * side;
		break;
	case 1:
		csgo->should_sidemove = false;
		if (!send_packet)
			csgo->cmd->viewangles.y -= 116.f * side;
		if (csgo->InLbyUpdate)
			csgo->cmd->viewangles.y -= 116.f * side;

		break;
	case 2:
		csgo->should_sidemove = true;
		if (!send_packet)
		{
			static auto st = 0;
			st++;
			if (st < 2)
			{
				static auto j = false;
				csgo->cmd->viewangles.y += j ? vars.antiaim.jitterrange : -vars.antiaim.jitterrange;
				j = !j;
			}
			else
				st = 0;
		}
		break;
	case 3:
		csgo->should_sidemove = true;
		if (!send_packet)
			csgo->cmd->viewangles.y += sw ? vars.antiaim.jitterrange : -vars.antiaim.jitterrange;
		sw = !sw;
		break;
	case 4:
		csgo->should_sidemove = true;
		if (!send_packet)
			csgo->cmd->viewangles.y += Math::RandomFloat(vars.antiaim.jitterrange, -vars.antiaim.jitterrange);
		break;

	}

}



void CMAntiAim::Run(bool& send_packet)
{

	if (!vars.antiaim.freestanding) {
		Freestanding();
	}

	if (vars.antiaim.slowwalk->active || csgo->should_stop_slide)
	{
		const auto weapon = csgo->weapon;

		if (weapon) {

			const auto info = csgo->weapon->GetCSWpnData();

			float speed = 0.1f;
			if (info) {
				float max_speed = weapon->GetZoomLevel() == 0 ? info->m_flMaxSpeed : info->m_flMaxSpeedAlt;
				float ratio = max_speed / 250.0f;
				speed *= ratio;
			}
			

			csgo->cmd->forwardmove *= speed;
			csgo->cmd->sidemove *= speed;
		}

		//csgo->should_stop_fast = false;
	}

	shouldAA = true;
	if (!vars.antiaim.enable) {
		shouldAA = false;
		return;
	}
	if (csgo->cmd->buttons & IN_USE)
	{
		shouldAA = false;
		return;
	}
	if (/*csgo->game_rules->IsFreezeTime() ||*/ csgo->local->GetMoveType() == MOVETYPE_NOCLIP || csgo->local->GetMoveType() == MOVETYPE_LADDER)
	{
		shouldAA = false;
		return;
	}
	bool shit = false;
	for (int i = 1; i < 65; i++)
	{
		auto ent = interfaces.ent_list->GetClientEntity(i);
		if (!ent)
			continue;
		if (
			!ent->isAlive()
			|| ent == csgo->local
			|| ent->GetTeam() == csgo->local->GetTeam()
			)
			continue;
		shit = true;
		break;

	}
	if (!shit)
	{
		if (csgo->ForceOffAA)
		{
			shouldAA = false;
			return;
		}
	}
	if (csgo->weapon->GetItemDefinitionIndex() == WEAPON_REVOLVER)
	{
		if (Ragebot::Get().shot /*&& Ragebot::Get().IsAbleToShoot()*/)
		{
			shouldAA = false;
			return;
		}
	}
	else
	{
		if (F::Shooting() || csgo->TickShifted || (CanDT() && csgo->cmd->buttons & IN_ATTACK /*&& Ragebot::Get().IsAbleToShoot()*/))
		{
			shouldAA = false;
			return;
		}
		if (csgo->weapon->IsKnife()) {
			if ((csgo->cmd->buttons & IN_ATTACK || csgo->cmd->buttons & IN_ATTACK2) && Ragebot::Get().IsAbleToShoot())
			{
				shouldAA = false;
				return;
			}
		}
	}
	if (shouldAA)
	{
		Pitch();
		Yaw(send_packet);
	}
}