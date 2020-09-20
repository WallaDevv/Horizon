#pragma once
#include "Hooks.h"

void util_trace_hull(Vector vec_start,  Vector vec_end, const unsigned int n_mask, Vector extents, trace_t* p_trace)
{
	CTraceFilterNoPlayers filter;

	Ray_t ray;
	ray.Init(vec_start, vec_end);
	ray.m_Extents = extents;
	ray.m_IsRay = false;

	interfaces.trace->TraceRay(ray, n_mask, &filter, p_trace);
}

void Thirdperson()
{
	if (!interfaces.engine->IsInGame() || !csgo->local)
		return;

	if (!enabledtp)
	{
		interfaces.input->m_fCameraInThirdPerson = false;
	}
	else
	{
		interfaces.input->m_fCameraInThirdPerson = true;
	}
	auto local = csgo->local;
	if (csgo->local->isAlive() && interfaces.input->m_fCameraInThirdPerson)
	{
		const auto get_correct_distance = [&local](float ideal_distance) -> float
		{
			Vector inverse_angles;
			interfaces.engine->GetViewAngles(inverse_angles);
			inverse_angles.x *= -1.f, inverse_angles.y += 180.f;
			Vector direction;
			Math::AngleVectors(inverse_angles, direction);
			trace_t trace;
			CTraceWorldOnly filter;
			Ray_t ray;
			ray.Init(local->GetEyePosition(), local->GetEyePosition() + direction * ideal_distance);
			interfaces.trace->TraceRay(ray, MASK_SHOT, &filter, &trace);
			return ideal_distance * trace.fraction - 10.f;
		};
		Vector angles;
		interfaces.engine->GetViewAngles(angles);
		angles.z = get_correct_distance(vars.visuals.thirdperson_dist);
		interfaces.input->m_vecCameraOffset = Vector(angles.x, angles.y, angles.z);

	}
	else
	{
		interfaces.input->m_vecCameraOffset = Vector(0, 0, 0);
		interfaces.input->m_fCameraInThirdPerson = false;
	}
		




	/*static auto b_once = false;

	if (csgo->local->isAlive())
	{
		b_once = false;
		return;
	}

	if (b_once)
		csgo->local->SetObserverMode(5);

	if (csgo->local->GetObserverMode() == 4)
		b_once = true;*/
}

void __stdcall Hooked_OverrideView(void* pSetup)
{
	

	if (csgo->DoUnload)
		return 	H::OverRideView(pSetup);

	//static ConVar* vFOV = interfaces.cvars->FindVar("viewmodel_fov");
	//vFOV->m_fnChangeCallbacks.m_Size = 0;
	//vFOV->SetValue(vars.misc.viewmodelfov);

	auto Setup = reinterpret_cast<CViewSetup*>(pSetup);

	if (interfaces.engine->IsInGame() && interfaces.engine->IsConnected() && !csgo->local->IsScoped())
	{		
		Setup->fov = vars.misc.worldfov;
	}
	
	Thirdperson();

	if (interfaces.engine->IsInGame() && interfaces.engine->IsConnected())
	{
		if (csgo->local && csgo->local->isAlive()) {

			if (vars.visuals.remove & 1)
				Setup->angles -= csgo->local->GetPunchAngle() * 0.9f + csgo->local->GetViewPunchAngle();
			if (vars.visuals.remove & 16)
			{
				if (csgo->weapon && csgo->weapon->isSniper() && csgo->local->IsScoped())
				{
					switch (csgo->weapon->GetZoomLevel())
					{
					case 1:
					{
						if (vars.misc.worldfov > 90)
							Setup->fov = vars.misc.worldfov;
						else
							Setup->fov = 90;
					}
					break;
					case 2:
					{
						if (vars.misc.worldfov > 90)
							Setup->fov = vars.misc.worldfov / 2;
						else
							Setup->fov = 45;
					}
					break;
					}

				
				}
			}

			if (csgo->fake_duck)
				Setup->origin.z = csgo->local->GetAbsOrigin().z + 64.f;

			if (vars.visuals.remove & 8 && csgo->local->IsScoped())
				Setup->m_iEdgeBlur = 0;

			CGrenadePrediction::Get().View(Setup);
		}
	
	}

	H::OverRideView(pSetup);
}

