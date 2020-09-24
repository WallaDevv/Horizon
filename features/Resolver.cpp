#include "../hooks/Hooks.h"
#include "Resolver.h"
#include "RageBacktracking.h"
#include "Ragebot.h"
#include "AnimationFix.h"


std::string ResolverMode[65];
int last_ticks[65];
int IBasePlayer::GetChokedPackets() {
	auto ticks = TIME_TO_TICKS(GetSimulationTime() - GetOldSimulationTime());
	if (ticks == 0 && last_ticks[GetIndex()] > 0) {
		return last_ticks[GetIndex()] - 1;
	}
	else {
		last_ticks[GetIndex()] = ticks;
		return ticks;
	}
}

float CResolver::GetAngle(IBasePlayer* player) {
	return Math::NormalizeYaw(player->GetEyeAngles().y);
}

float CResolver::GetForwardYaw(IBasePlayer* player) {
	return Math::NormalizeYaw(GetBackwardYaw(player) - 180.f);
}

float CResolver::GetBackwardYaw(IBasePlayer* player) {
	return Math::CalculateAngle(csgo->local->GetOrigin(), player->GetOrigin()).y;
}

float CResolver::GetLeftYaw(IBasePlayer* player) {
	return Math::NormalizeYaw(Math::CalculateAngle(csgo->local->GetOrigin(), player->GetOrigin()).y - 360.f);
}

float CResolver::GetRightYaw(IBasePlayer* player) {
	return Math::NormalizeYaw(Math::CalculateAngle(csgo->local->GetOrigin(), player->GetOrigin()).y + 360.f);
}
bool CResolver::TargetJitter(IBasePlayer* player, bool v2) {
	float yaw = v2 ? GetRightYaw(player) : GetLeftYaw(player);
	return fabsf(GetAngle(player) - Math::NormalizeYaw(yaw + 360.f))
		>= fabsf(GetAngle(player) - Math::NormalizeYaw(yaw - 360.f));
}
bool CResolver::TargetSide(IBasePlayer* player) {
	float yaw = Math::NormalizeYaw(GetBackwardYaw(player));
	float angle = GetAngle(player);
	return fabsf(angle - Math::NormalizeYaw(yaw + 60.f))
		>= fabsf(angle - Math::NormalizeYaw(yaw - 60.f));
}

void CResolver::DetectSide(IBasePlayer* player, int* side)
{
	Vector src3D, dst3D, forward, right, up, src, dst;
	float back_two, right_two, left_two;
	trace_t tr;
	Ray_t ray, ray2, ray3, ray4, ray5, ray6, ray7;
	CTraceFilter filter;

	Math::AngleVectors(Vector(0, GetBackwardYaw(player), 0), &forward, &right, &up);

	filter.pSkip = player;
	src3D = player->GetEyePosition();
	dst3D = src3D + (forward, right, up* 980); //Might want to experiment with other numbers, incase you don't know what the number does, its how far the trace will go. Lower = shorter.
	ray.Init(src3D, dst3D);
	interfaces.trace->TraceRay(ray, MASK_SHOT, &filter, &tr);
	back_two = (tr.endpos - tr.startpos).Length();

	ray2.Init(src3D + right * 30, dst3D + right * 30);
	interfaces.trace->TraceRay(ray2, MASK_SHOT, &filter, &tr);
	right_two = (tr.endpos - tr.startpos).Length();

	ray3.Init(src3D - right * 70, dst3D - right * 70);
	interfaces.trace->TraceRay(ray3, MASK_SHOT, &filter, &tr);
	left_two = (tr.endpos - tr.startpos).Length();

	ray4.Init(src3D - right * 90, dst3D - right * 90);
	interfaces.trace->TraceRay(ray3, MASK_SHOT, &filter, &tr);
	right_two = (tr.endpos - tr.startpos).Length();

	ray5.Init(src3D - right * 30, dst3D - right * 30);
	interfaces.trace->TraceRay(ray3, MASK_SHOT, &filter, &tr);
	right_two = (tr.endpos - tr.startpos).Length();

	ray6.Init(src3D - right * 30, dst3D - right * 30);
	interfaces.trace->TraceRay(ray3, MASK_SHOT, &filter, &tr);
	left_two = (tr.endpos - tr.startpos).Length();

	ray7.Init(src3D + right * 90, dst3D + right * 90);
	interfaces.trace->TraceRay(ray, MASK_SHOT, &filter, &tr);
	back_two = (tr.endpos - tr.startpos).Length();

	if (left_two > right_two) {
		*side = -1;
		//Body should be right
	}
	else if (right_two > left_two) {
		*side = 1;
	}
	else
		*side = 0;
}

bool CResolver::DoesHaveJitter(IBasePlayer* player, int* new_side) {
	static float LastAngle[64];
	static int LastBrute[64];
	static bool Switch[64];
	static float LastUpdateTime[64];

	int i = player->GetIndex();

	float CurrentAngle = player->GetEyeAngles().y;
	if (!Math::IsNearEqual(CurrentAngle, LastAngle[i], 60.f)) {
		Switch[i] = !Switch[i];
		LastAngle[i] = CurrentAngle;
		*new_side = Switch[i] ? 1 : -1;
		LastBrute[i] = *new_side;
		LastUpdateTime[i] = interfaces.global_vars->curtime;
		return true;
	}
	else {
		if (fabsf(LastUpdateTime[i] - interfaces.global_vars->curtime >= TICKS_TO_TIME(17))
			|| player->GetSimulationTime() != player->GetOldSimulationTime()) {
			LastAngle[i] = CurrentAngle;
		}
		*new_side = LastBrute[i];
	}
	return false;
}


void CResolver::StoreAntifreestand()
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
					FreestandSide[idx] = -1;
				}
				else if (side == 1)
				{
					HitSide2 = true;
					FreestandSide[idx] = 1;
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
							FreestandSide[idx] = -1;
							//FreestandAngle[pPlayerEntity->EntIndex()] = 90;
						}
						else if (sidealternative == 1)
						{
							HitSide2 = true;
							FreestandSide[idx] = 1;
							//FreestandAngle[pPlayerEntity->EntIndex()] = -90;
						}

						Autowalled = true;
					}
				}
			}
		}
	}
}


void CResolver::Do(IBasePlayer* player) {
	auto animstate = player->GetPlayerAnimState();
	if (!animstate)
		return;
	animstate->m_flGoalFeetYaw = GetAngle(player);
	if (!vars.ragebot.resolver)
		return;
	if (!csgo->local->isAlive())
		return;
	if (player->GetChokedPackets() <= 0)
		return;

	int idx = player->GetIndex();

	if (player->GetPlayerInfo().fakeplayer)
		return;

	if (!(player->GetFlags() & FL_ONGROUND))
		return;

	float angle = GetAngle(player);
	int new_side = 0;
	if (DoesHaveJitter(player, &new_side) && player->GetEyeAngles().x < 58) {
	 switch (csgo->missedshots[idx] % 2) {
	 case 0:
	 animstate->m_flGoalFeetYaw = Math::NormalizeYaw(angle + 100.f * new_side);
	 break;
	 case 1:
	 animstate->m_flGoalFeetYaw = Math::NormalizeYaw(angle - 100.f * new_side);
	 break;
	 }
	 }
	 else
	{
		bool forward = fabsf(Math::NormalizeYaw(GetAngle(player) - GetForwardYaw(player))) < 89.f;

		animstate->m_flGoalFeetYaw = Math::NormalizeYaw(animstate->m_flGoalFeetYaw);

		float angles = GetAngle(player);
		int new_side = 0;
		{
			bool forward = fabsf(Math::NormalizeYaw(GetAngle(player) - GetForwardYaw(player))) < 90.f;
			if (csgo->missedshots[idx] == 1) {
			}
			else {
				if (forward) {
					switch (csgo->missedshots[idx] == 1) {

					case 0:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 120.0;
						break;
					case 1:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 120.0;
						break;
					case 2:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 15.0;
						break;
					case 3:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 15.0;
						break;
					case 4:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 90.0;
						break;
					case 5:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 90.0;
						break;
					case 6:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 115.0;
						break;
					case 7:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 115.0;
						break;
					case 8:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 70.0;
						break;
					case 9:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 70.0;
						break;
					case 10:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 60.0;
						break;
					case 11:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 60.0;
						break;
					case 12:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 30.0;
						break;
					case 13:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 30.0;
						break;
					case 14:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 58.0;
						break;
					case 15:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 58.0;
						break;
					case 16:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 45.0;
						break;
					case 17:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 45.0;
						break;
					case 18:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 20.0;
						break;
					case 19:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 20.0;
						break;
					case 20:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 5.0;
						break;
					case 21:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 5.0;
						break;
					case 22:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 180.0;
						break;
					case 23:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 180.0;
						break;
					case 24:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 50.0;
						break;
					case 25:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 50.0;
						break;
					case 26:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 25.0;
						break;
					case 27:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 25.0;
						break;
					case 28:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 130.0;
						break;
					case 29:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 130.0;
						break;
					case 30:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 150.0;
						break;
					case 31:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 150.0;
						break;
					case 32:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw + 55.0;
						break;
					case 33:
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw - 55.0;
						break;
					case 34: 
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw;
						break;
					case 35:  
						animstate->m_flGoalFeetYaw = animstate->m_flGoalFeetYaw;
						break;
					case 36: 
						animstate->m_flGoalFeetYaw += animstate->AbsYaw() + 60;
						break;
					case 37: 
						animstate->m_flGoalFeetYaw += animstate->AbsYaw() - 60;
						break;
					case 38: 
						animstate->m_flGoalFeetYaw += 30;
						break;
					case 39: 
						animstate->m_flGoalFeetYaw -= 30;
					}

					animstate->m_flGoalFeetYaw = Math::NormalizeYaw(animstate->m_flEyeYaw);

					animstate->m_flGoalFeetYaw = Math::NormalizeYaw(animstate->m_flGoalFeetYaw);
						
					
				}
			}
		}
	}
}