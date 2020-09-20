#include "Visuals.h"
#include "Autowall.h"
#include "Resolver.h"
#include "AntiAims.h"
#include "Ragebot.h"
#include "../render.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../shonax things/c_renderer.h"
#include "Features.h"
bool GetBox(IBasePlayer* entity, int& x, int& y, int& w, int& h, Vector origin)
{
	if (entity->GetClientClass() && entity->GetClientClass()->m_ClassID == ClassId->CCSPlayer) {
		auto min = entity->GetCollideable()->OBBMins();
		auto max = entity->GetCollideable()->OBBMaxs();

		Vector dir, vF, vR, vU;

		interfaces.engine->GetViewAngles(dir);
		dir.x = 0;
		dir.z = 0;
		//dir.Normalize();
		//printf("%.1f\n", dir.y);
		Math::AngleVectors(dir, &vF, &vR, &vU);

		auto zh = vU * max.z + vF * max.y + vR * min.x; // = Front left front
		auto e = vU * max.z + vF * max.y + vR * max.x; //  = Front right front
		auto d = vU * max.z + vF * min.y + vR * min.x; //  = Front left back
		auto c = vU * max.z + vF * min.y + vR * max.x; //  = Front right back

		auto g = vU * min.z + vF * max.y + vR * min.x; //  = Bottom left front
		auto f = vU * min.z + vF * max.y + vR * max.x; //  = Bottom right front
		auto a = vU * min.z + vF * min.y + vR * min.x; //  = Bottom left back
		auto b = vU * min.z + vF * min.y + vR * max.x; //  = Bottom right back*-

		Vector pointList[] = {
			a,
			b,
			c,
			d,
			e,
			f,
			g,
			zh,
		};

		Vector2D transformed[ARRAYSIZE(pointList)];

		for (int i = 0; i < ARRAYSIZE(pointList); i++)
		{
			pointList[i] += origin;

			//if (!Math::WorldToScreen2(pointList[i], transformed[i]))
			//	return false;
			//c_renderer::world_to_screen_matrix();
			if (!c_renderer::screen_transform(pointList[i], transformed[i], features->Visuals->world_to_screen_matrix))
				return false;

			//screen_transform
		}

		float left = FLT_MAX;
		float top = -FLT_MAX;
		float right = -FLT_MAX;
		float bottom = FLT_MAX;
		for (int i = 0; i < ARRAYSIZE(pointList); i++) {
			if (left > transformed[i].x)
				left = transformed[i].x;
			if (top < transformed[i].y)
				top = transformed[i].y;
			if (right < transformed[i].x)
				right = transformed[i].x;
			if (bottom > transformed[i].y)
				bottom = transformed[i].y;
		}

		x = left;
		y = bottom;
		w = right - left;
		h = top - bottom;

		return true;
	}
	else
	{
		Vector vOrigin, min, max;
		//float left, top, right, bottom;
		Vector2D flb, blb, brt, frt, frb, brb, blt, flt;

		auto collideable = entity->GetCollideable();

		if (!collideable)
			return false;

		min = collideable->OBBMins();
		max = collideable->OBBMaxs();

		auto& trans = entity->GetrgflCoordinateFrame();

		Vector points[] =
		{
			Vector(min.x, min.y, min.z),
			Vector(min.x, max.y, min.z),
			Vector(max.x, max.y, min.z),
			Vector(max.x, min.y, min.z),
			Vector(max.x, max.y, max.z),
			Vector(min.x, max.y, max.z),
			Vector(min.x, min.y, max.z),
			Vector(max.x, min.y, max.z)
		};

		Vector pointsTransformed[8];
		for (int i = 0; i < 8; i++) {
			Math::VectorTransform(points[i], trans, pointsTransformed[i]);
		}

		Vector pos = entity->GetRenderOrigin();

		if (!c_renderer::screen_transform(pointsTransformed[3], flb, features->Visuals->world_to_screen_matrix) || !c_renderer::screen_transform(pointsTransformed[5], brt, features->Visuals->world_to_screen_matrix)
			|| !c_renderer::screen_transform(pointsTransformed[0], blb, features->Visuals->world_to_screen_matrix) || !c_renderer::screen_transform(pointsTransformed[4], frt, features->Visuals->world_to_screen_matrix)
			|| !c_renderer::screen_transform(pointsTransformed[2], frb, features->Visuals->world_to_screen_matrix) || !c_renderer::screen_transform(pointsTransformed[1], brb, features->Visuals->world_to_screen_matrix)
			|| !c_renderer::screen_transform(pointsTransformed[6], blt, features->Visuals->world_to_screen_matrix) || !c_renderer::screen_transform(pointsTransformed[7], flt, features->Visuals->world_to_screen_matrix))
			return false;

		Vector2D arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };
		//+1 for each cuz of borders at the original box
		float left = flb.x;        // left
		float top = flb.y;        // top
		float right = flb.x;    // right
		float bottom = flb.y;    // bottom

		for (int i = 1; i < 8; i++)
		{
			if (left > arr[i].x)
				left = arr[i].x;
			if (bottom < arr[i].y)
				bottom = arr[i].y;
			if (right < arr[i].x)
				right = arr[i].x;
			if (top > arr[i].y)
				top = arr[i].y;
		}

		x = (int)left;
		y = (int)top;
		w = (int)(right - left);
		h = (int)(bottom - top);

		return true;
	}
	return false;
}

void CVisuals::DrawAngleLine(Vector origin, float angle, color_t color)
{
	Vector src, dst, sc1, sc2, forward;

	src = origin;
	Math::AngleVectors(Vector(0, angle, 0), &forward);
	if (Math::WorldToScreen(src, sc1) && Math::WorldToScreen(src + (forward * 40), sc2))
	{
		Drawing::DrawLine(sc1.x, sc1.y, sc2.x, sc2.y, color);
		//g_Render->DrawLine(sc1.x, sc1.y, sc2.x, sc2.y, color, 1.f);
	}
}

void CVisuals::DrawLocalShit(IDirect3DDevice9* pDevice)
{
	if (csgo->local && csgo->local->isAlive() && csgo->weapon)
	{
		int dx = csgo->w, dy = csgo->h;

		if (vars.visuals.remove & 8)
		{
			if (csgo->weapon->isSniper() && csgo->local->IsScoped())
			{
				g_Render->DrawLine(dx / 2, 0, dx / 2, dy, color_t(0, 0, 0, 150), vars.visuals.scope_thickness);
				g_Render->DrawLine(0, dy / 2, dx, dy / 2, color_t(0, 0, 0, 150), vars.visuals.scope_thickness);
			}
		}

		dx /= 2;
		dy /= 2;

		if (vars.visuals.innacuracyoverlay)
		{
			if (vars.visuals.innacuracyoverlaytype == 0)
			{
				g_Render->circle_filled_radial_gradient(pDevice, Vector2D(dx, dy), csgo->weaponspread,
					vars.visuals.innacuracyoverlay_color, color_t(
					vars.visuals.innacuracyoverlay_color[0],
					vars.visuals.innacuracyoverlay_color[1],
					vars.visuals.innacuracyoverlay_color[2],
					100));
			}
			else
			{
				g_Render->circle_filled_rainbow(pDevice, Vector2D(dx, dy), csgo->weaponspread, true, vars.visuals.innacuracyoverlay_rotatingspeed / 100.f);
			}
		}

		static auto percent_col = [](int per) -> color_t {
			int red = per < 50 ? 255 : floorf(255 - (per * 2 - 100) * 255.f / 100.f);
			int green = per > 50 ? 255 : floorf((per * 2) * 255.f / 100.f);

			return color_t(red, green, 0);
		};

		int add = 0;

		auto anims = csgo->local->GetPlayerAnimState();

		if (vars.antiaim.enable && !csgo->ForceOffAA)
		{
			if (csgo->should_stop)
			{
				auto clr = percent_col(csgo->delta);
				g_Render->DrawString(10, dx - (50 + add), color_t(clr.get_alpha() - 126, clr.get_green() - 65, 10),
					render::outline, fonts::lby_indicator, "STOP"
				);
				//g_Render->DrawString(fonts::lby_indicator, 10, dx - (50 + add), false, false, true, color_t(clr.a() - 126, clr.g() - 65, 10), "FAKE");
				add += 30;
			}
			if (vars.visuals.indicators & 1 && vars.antiaim.enable)
			{
				auto clr = percent_col(csgo->delta);
				g_Render->DrawString(10, dx - (50 + add), color_t(clr.get_alpha() - 126, clr.get_green() - 65, 10),
					render::outline, fonts::lby_indicator, "FAKE"
				);
				//g_Render->DrawString(fonts::lby_indicator, 10, dx - (50 + add), false, false, true, color_t(clr.a() - 126, clr.g() - 65, 10), "FAKE");
				add += 30;
			}
			if (vars.visuals.indicators & 2 && vars.antiaim.fakelag > 0)
			{
				static color_t clr;
				if (csgo->canDrawLC)
				{
					clr = csgo->canBreakLC ? color_t(129, 190, 10) : color_t(255, 0, 0);
					g_Render->DrawString(10, dx - (50 + add), clr,
						render::outline, fonts::lby_indicator, "LC"
					);
					add += 30;
				}
			}
			if (vars.visuals.indicators & 8 && vars.antiaim.fakeduck->active)
			{
				//g_Render->DrawString(fonts::lby_indicator, 10, dx - (50 + add), false, false, true, color_t(129, 190, 10), "FD");
				g_Render->DrawString(10, dx - (50 + add), color_t(129, 190, 10),
					render::outline, fonts::lby_indicator, "FD"
				);
				add += 30;
			}
			if (vars.antiaim.aa_override.enable && vars.visuals.indicators & 64)
			{
				//static bool left, right, back;
				//if (csgo->key_pressed[vars.antiaim.aa_override.left])
				//{
				//	left = true;
				//	right = false;
				//	back = false;
				//}
				//else if (csgo->key_pressed[vars.antiaim.aa_override.right])
				//{
				//	left = false;
				//	right = true;
				//	back = false;
				//}
				//else if (csgo->key_pressed[vars.antiaim.aa_override.back])
				//{
				//	left = false;
				//	right = false;
				//	back = true;
				//}

				//if (left)
				//{
				//	//g_Render->DrawString(fonts::lby_indicator, 10, dx - (50 + add), false, false, true, color_t(129, 190, 10), "LEFT");
				//	g_Render->DrawString(10, dx - (50 + add), color_t(129, 190, 10),
				//		render::outline, fonts::lby_indicator, "LEFT"
				//	);
				//	add += 30;
				//}
				//if (right)
				//{
				//	g_Render->DrawString(10, dx - (50 + add), color_t(129, 190, 10),
				//		render::outline, fonts::lby_indicator, "RIGHT"
				//	);
				//	add += 30;
				//}
				//if (back)
				//{
				//	g_Render->DrawString(10, dx - (50 + add), color_t(129, 190, 10),
				//		render::outline, fonts::lby_indicator, "BACK"
				//	);
				//	add += 30;
				//}
			}
		}
		if (vars.ragebot.enable)
		{
			if (vars.visuals.indicators & 8 && vars.ragebot.override_dmg->active) {

				//g_Render->DrawString(fonts::lby_indicator, 10, dx - (50 + add), false, false, true, color_t(129, 190, 10), "DMG");
				g_Render->DrawString(10, dx - (50 + add), color_t(129, 190, 10),
					render::outline, fonts::lby_indicator, "DMG"
				);
				add += 30;
			}
			if (vars.visuals.indicators & 16 && vars.ragebot.force_body->active) {
				//g_Render->DrawString(fonts::lby_indicator, 10, dx - (50 + add), false, false, true, color_t(129, 190, 10), "BAIM");
				g_Render->DrawString(10, dx - (50 + add), color_t(129, 190, 10),
					render::outline, fonts::lby_indicator, "BAIM"
				);
				add += 30;
			}
			if (vars.visuals.indicators & 32 && csgo->dt_charged) {
				//g_Render->DrawString(fonts::lby_indicator, 10, dx - (50 + add), false, false, true, color_t(129, 190, 10), "DT");
				if (CanDT() && !CMAntiAim::Get().did_shot) {
					g_Render->DrawString(10, dx - (50 + add), color_t(129, 190, 10),
						render::outline, fonts::lby_indicator, "DT"
					);
					add += 30;
				}
				if (CanHS()) {
					g_Render->DrawString(10, dx - (50 + add), color_t(129, 190, 10),
						render::outline, fonts::lby_indicator, "HS"
					);
					add += 30;
				}
			}
		}
	}
}

std::string str_toupper(std::string s) {
	std::transform(s.begin(), s.end(), s.begin(),
		[](unsigned char c) { return toupper(c); }
	);
	return s;
}

void DrawFovArrows(IBasePlayer* entity, float alpha, Vector origin)
{
	if (!vars.visuals.out_of_fov)
		return;

	Vector view;
	interfaces.engine->GetViewAngles(view);
	Vector vEnemyOrigin = origin;
	Vector vLocalOrigin = csgo->origin;
	auto angle_to = Math::CalculateAngle(vLocalOrigin, vEnemyOrigin);
	
	auto target_angle = angle_to - view;
	Math::normalize(target_angle);

	const auto angle = target_angle.y;
	const auto height = 400.f;
	#define deg2rad(x) ((float)(x) * (float)(3.141593f / 180.f))

	auto clr = entity->IsDormant() ? color_t(50, 50, 50, alpha - 50) : color_t(
		vars.visuals.out_of_fov_color[0],
		vars.visuals.out_of_fov_color[1],
		vars.visuals.out_of_fov_color[2],
		vars.visuals.out_of_fov_color[3] * (alpha / 255.f));


	const auto a = c_renderer::get_center() - Vector2D(
		(height - 20) * sin(deg2rad(angle + 2)),
		(height - 20) * cos(deg2rad(angle + 2))
	);

	const auto b = c_renderer::get_center() - Vector2D(
		height * sin(deg2rad(angle)),
		height * cos(deg2rad(angle))
	);

	const auto c = c_renderer::get_center() - Vector2D(
		(height - 20) * sin(deg2rad(angle - 2)),
		(height - 20) * cos(deg2rad(angle - 2))
	);

	c_renderer::triangle_filled_linear_gradient(a, b, c, color_t(40, 40, 40, 200), clr, clr);
	c_renderer::triangle(a, b, c, color_t(0, 0, 0, 255));

}


void CVisuals::Draw()
{
	if (!interfaces.engine->IsConnected() && !interfaces.engine->IsInGame())
		return;

	

	//Drawing::DrawBox++
	if (vars.visuals.enable)
	{



		for (auto i = 0; i < interfaces.global_vars->maxClients; i++)
		{
			IBasePlayer* entity = interfaces.ent_list->GetClientEntity(i);
			if (!entity)
				continue;

			{

				if (entity == nullptr
					|| !entity->IsPlayer()
					|| entity == csgo->local
					|| entity->GetTeam() == csgo->local->GetTeam())
					continue;

				bool dormant = entity->IsDormant();
				if (vars.visuals.dormant)
				{
					if (!entity->isAlive())
					{
						csgo->EntityAlpha[entity->GetIndex()] = 0;
						continue;
					}
					strored_origin[entity->GetIndex()] = entity->GetRenderOrigin();
					if (dormant)
					{
						if (csgo->EntityAlpha[entity->GetIndex()] > 140)
							csgo->EntityAlpha[entity->GetIndex()]--;
						else if ((csgo->LastSeenTime[entity->GetIndex()] + 3.f) <= csgo->get_absolute_time() && csgo->EntityAlpha[entity->GetIndex()] > 0)
							csgo->EntityAlpha[entity->GetIndex()]--;

						if (csgo->EntityAlpha[entity->GetIndex()] <= 0.f)
							continue;
					}
					else
					{
						csgo->EntityAlpha[entity->GetIndex()] = 255;
						csgo->LastSeenTime[entity->GetIndex()] = csgo->get_absolute_time();
					}

					csgo->EntityAlpha[entity->GetIndex()] = std::clamp(csgo->EntityAlpha[entity->GetIndex()], 0.f, 255.f);
				}
				else
				{
					if (dormant || !entity->isAlive())
						continue;

					strored_origin[entity->GetIndex()] = entity->GetRenderOrigin();
				}

				DrawFovArrows(entity, csgo->EntityAlpha[entity->GetIndex()], strored_origin[entity->GetIndex()]);

				int alpha = vars.visuals.dormant ? csgo->EntityAlpha[entity->GetIndex()] : 255;

				auto weapon = entity->GetWeapon();
				if (!weapon)
					continue;

				BOX box;
				if (!GetBox(entity, box.x, box.y, box.w, box.h, strored_origin[entity->GetIndex()]))
					continue;

				if (vars.visuals.box)
				{
					auto clr = dormant ? color_t(255, 255, 255, alpha - 50) : color_t(
						vars.visuals.box_color[0],
						vars.visuals.box_color[1],
						vars.visuals.box_color[2],
						alpha - 50);

					c_renderer::rect(Vector2D(box.x, box.y), Vector2D(box.w, box.h), clr);
					c_renderer::rect(Vector2D(box.x - 1, box.y - 1), Vector2D(box.w + 2, box.h + 2), color_t(10, 10, 10, (alpha * 0.8f)));
					c_renderer::rect(Vector2D(box.x + 1, box.y + 1), Vector2D(box.w - 2, box.h - 2), color_t(10, 10, 10, (alpha * 0.8f)));
				}

				int count = 0;
				if (vars.visuals.name)
				{
					auto text_size = c_renderer::get_text_size(entity->GetName().c_str(), fnv1a("prime12"));
					int r = vars.visuals.name_color[0],
						g = vars.visuals.name_color[1],
						b = vars.visuals.name_color[2],
						a = vars.visuals.name_color[3] * (alpha / 255.f);

					auto clr = dormant ? color_t(255, 255, 255, alpha) : color_t(r, g, b, a);

					c_renderer::text(Vector2D(box.x + box.w / 2 - text_size.x / 2, box.y - text_size.y / 2 - 10), entity->GetName().c_str(), clr, fnv1a("prime12"));
				}
				if (!dormant) {
					auto record = g_Animfix->get_latest_animation(entity);
					if (record.has_value() && record.value()->player) {
						auto clr = color_t(
							vars.visuals.skeleton_color[0],
							vars.visuals.skeleton_color[1],
							vars.visuals.skeleton_color[2],
							alpha - 50);
						if (vars.visuals.shot_multipoint) {

							auto hitboxes = Ragebot::Get().GetHitboxesToScan(record.value()->player);
							auto priority = Ragebot::Get().GetCurrentPriorityHitbox(record.value()->player);
							for (auto i : hitboxes) {
								auto points = Ragebot::Get().GetMultipoints(record.value()->player, i, record.value()->bones);
								for (auto p : points) {
									Vector2D world;
									if (c_renderer::screen_transform(p, world, features->Visuals->world_to_screen_matrix)) {
										Drawing::DrawBox(world.x - 2, world.y - 2, 4, 4, color_t(0, 0, 0, alpha));
										Drawing::DrawBox(world.x - 1, world.y - 1, 2, 2, priority == i ? color_t(255, 150, 0, alpha) : color_t(255, 255, 255, alpha));

									}
								}
							}
						}
						if (vars.visuals.skeleton) {
							studiohdr_t* pStudioHdr = interfaces.models.model_info->GetStudioModel(entity->GetModel());

							if (!pStudioHdr)
								return;

							Vector vParent, vChild;
							Vector2D sParent, sChild;

							for (int j = 0; j < pStudioHdr->numbones; j++)
							{
								mstudiobone_t* pBone = pStudioHdr->pBone(j);

								if (pBone && (pBone->flags & 0x100) && (pBone->parent != -1))
								{
									vChild = entity->GetBonePos(record.value()->bones, j);
									vParent = entity->GetBonePos(record.value()->bones, pBone->parent);

									if (c_renderer::screen_transform(vParent, sParent, features->Visuals->world_to_screen_matrix) && c_renderer::screen_transform(vChild, sChild, features->Visuals->world_to_screen_matrix))
										c_renderer::line(Vector2D(sParent.x, sParent.y), Vector2D(sChild.x, sChild.y), clr);

								}
							}
						}
					}
				}

				if (vars.visuals.healthbar)
				{
					int hp = entity->GetHealth();

					if (hp > 100)
						hp = 100;

					int hp_percent = box.h - (int)((box.h * hp) / 100);

					int width = (box.w * (hp / 100.f));

					int red = 255 - (hp * 2.55);
					int green = hp * 2.55;

					char hps[10] = "";

					sprintf_s(hps, "%i", hp);

					auto text_size = c_renderer::get_text_size(entity->GetName().c_str(), fnv1a("prime12"));

					auto clr = dormant ? color_t(200, 200, 200, alpha) : color_t(255, 255, 255, alpha);

					auto hp_color = dormant ? color_t(200, 200, 200, alpha) : vars.visuals.override_hp ? color_t(
						vars.visuals.hp_color[0],
						vars.visuals.hp_color[1],
						vars.visuals.hp_color[2],
						vars.visuals.hp_color[3] * (alpha / 255.f)) : color_t(red, green, 0, alpha);


					c_renderer::rect(Vector2D(box.x - 6, box.y - 1), Vector2D(4, box.h + 2), color_t(80, 80, 80, alpha * 0.49f));
					c_renderer::rect_filled(Vector2D(box.x - 6, box.y - 1), Vector2D(4, box.h + 2), color_t(10, 10, 10, (alpha * 0.8f)));
					c_renderer::rect(Vector2D(box.x - 5, box.y + hp_percent), Vector2D(2, box.h - hp_percent), hp_color);
					//if (hp <= 99) 
					//	c_renderer::text(Vector2D(box.x - text_size.x, box.y + hp_percent - 1), hps, color_t(255, 255, 255, alpha), fnv1a("prime10"));
				}


				if (weapon && !dormant)
				{
					if (weapon->IsGun())
					{

						auto ammo = weapon->GetAmmo(false);
						auto max_ammo = weapon->GetAmmo(true);
						if (vars.visuals.ammo)
						{
							auto clr = color_t(
								vars.visuals.ammo_color[0],
								vars.visuals.ammo_color[1],
								vars.visuals.ammo_color[2],
								vars.visuals.ammo_color[3] * (alpha / 255.f));

							int hp_percent = box.w - (int)((box.w * ammo) / 100);

							int width = (box.w * (ammo / float(max_ammo)));

							char ammostr[10];
							sprintf_s(ammostr, "%d", ammo);

							const auto text_size = c_renderer::get_text_size(ammostr, fnv1a("prime10"));

							c_renderer::rect(Vector2D(box.x, box.y + 2 + box.h), Vector2D(box.w + 1, 4), color_t(80, 80, 80, alpha * 0.49f));
							c_renderer::rect_filled(Vector2D(box.x, box.y + 2 + box.h), Vector2D(box.w + 1, 4), color_t(10, 10, 10, (alpha * 0.8f)));
							c_renderer::rect(Vector2D(box.x + 1, box.y + 3 + box.h), Vector2D(width - 1, 2), clr);

							//if (ammo < (max_ammo / 2) && ammo > 0)
							//	Drawing::DrawString(fonts::esp_info, box.x + width - 1 - text_size.right, box.y + box.h,
							//		color_t(255, 255, 255, alpha - 55.f), FONT_LEFT, ammostr);

							count++;
						}
					}
					if (vars.visuals.weapon)
					{
						auto clr = vars.visuals.weapon_color;

						auto weapon = entity->GetWeapon();
						if (!weapon)
							continue;

						auto weap_info = weapon->GetCSWpnData();
						if (!weap_info)
							continue;

						char wpn_name[100] = "";

						sprintf_s(wpn_name, "%s", str_toupper(weap_info->m_szWeaponName).c_str() + 7);

						if (entity->GetWeapon()->GetItemDefinitionIndex() == 64)
							strcpy_s(wpn_name, "REVOLVER");

						auto wpn_name_size = c_renderer::get_text_size(wpn_name, fnv1a("prime10"));

						c_renderer::text(Vector2D(box.x + box.w / 2 - wpn_name_size.x / 2, box.y + 1 + box.h + (count++ * 6.f)), wpn_name, clr, fnv1a("prime12"));
					}

				}

				int offset = 0;
				if (weapon && !dormant)
				{
					auto clr = vars.visuals.flags_color;

					if (vars.visuals.flags & 1 && entity->GetArmor() > 0) {
						if (entity->HasHelmet())
							c_renderer::text(Vector2D(box.x + box.w + 3, box.y + offset++ * 9), "HK", clr, fnv1a("prime12"));
						else
							c_renderer::text(Vector2D(box.x + box.w + 3, box.y + offset++ * 9), "K", clr, fnv1a("prime12"));
					}
					if (vars.visuals.flags & 2 && entity->IsScoped()) {
						c_renderer::text(Vector2D(box.x + box.w + 3, box.y + offset++ * 9), "Zoom", clr, fnv1a("prime12"));
					}
					if (vars.visuals.flags & 4 && entity->IsFlashed() > 0) {
						c_renderer::text(Vector2D(box.x + box.w + 3, box.y + offset++ * 9), "Blind", clr, fnv1a("prime12"));
					}
					if (vars.visuals.flags & 8 && csgo->hitchance) {
						c_renderer::text(Vector2D(box.x + box.w + 3, box.y + offset++ * 9), "Hit", clr, fnv1a("prime12"));
						csgo->hitchance = false;
					}

					if (vars.visuals.flags & 16 && vars.ragebot.resolver && ResolverMode[entity->EntIndex()] != "") {
						c_renderer::text(Vector2D(box.x + box.w + 3, box.y + offset++ * 9), ResolverMode[entity->EntIndex()].c_str(), clr, fnv1a("prime12"));
					}
					if (vars.visuals.flags & 32 && vars.ragebot.resolver) {
						auto str = "CHOKE: " + std::to_string(entity->GetChokedPackets());
						c_renderer::text(Vector2D(box.x + box.w + 3, box.y + offset++ * 9), str.c_str(), clr, fnv1a("prime12"));

					}

				}
			}

			
		}






		
	}


}

bool CVisuals::IsValidSound(SndInfo_t& sound) {
	// We don't want the sound to keep following client's predicted origin.
	for (int iter = 0; iter < m_utlvecSoundBuffer.Count(); iter++)
	{
		SndInfo_t& cached_sound = m_utlvecSoundBuffer[iter];
		if (cached_sound.m_nGuid == sound.m_nGuid)
		{
			return false;
		}
	}

	return true;
}

void CVisuals::OnDormant() {
	if (!interfaces.engine->IsInGame() || !csgo->local || !csgo->local->isAlive() || csgo->disable_dt)
		return;

	m_utlCurSoundList.RemoveAll();
	interfaces.engine_sound->GetActiveSounds(m_utlCurSoundList);

	// No active sounds.
	if (!m_utlCurSoundList.Count())
		return;

	// Accumulate sounds for esp correction
	for (int iter = 0; iter < m_utlCurSoundList.Count(); iter++)
	{
		SndInfo_t& sound = m_utlCurSoundList[iter];
		if (sound.m_nSoundSource == 0 || // World
			sound.m_nSoundSource > 64)   // Most likely invalid
			continue;

		auto player = interfaces.ent_list->GetClientEntity(sound.m_nSoundSource);

		if (!player || player == csgo->local || sound.m_pOrigin->IsZero())
			continue;

		if (!IsValidSound(sound))
			continue;

		AdjustDormant(player, sound);

		csgo->LastSeenTime[player->GetIndex()] = csgo->get_absolute_time();

		m_cSoundPlayers[sound.m_nSoundSource - 1].Override(sound);
	}

	m_utlvecSoundBuffer = m_utlCurSoundList;
}

void CVisuals::AdjustDormant(IBasePlayer* player, SndInfo_t& sound)
{
	Vector src3D, dst3D;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = player;
	src3D = (*sound.m_pOrigin) + Vector(0, 0, 1); // So they dont dig into ground incase shit happens /shrug
	dst3D = src3D - Vector(0, 0, 100);
	ray.Init(src3D, dst3D);

	interfaces.trace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &tr);

	// step = (tr.fraction < 0.20)
	// shot = (tr.fraction > 0.20)
	// stand = (tr.fraction > 0.50)
	// crouch = (tr.fraction < 0.50)

	/* Corrects origin and important flags. */

	// Player stuck, idk how this happened
	if (tr.allsolid)
	{
		m_cSoundPlayers[sound.m_nSoundSource - 1].m_iReceiveTime = -1;
	}

	*sound.m_pOrigin = ((tr.fraction < 0.97) ? tr.endpos : *sound.m_pOrigin);
	m_cSoundPlayers[sound.m_nSoundSource - 1].m_nFlags = player->GetFlags();
	m_cSoundPlayers[sound.m_nSoundSource - 1].m_nFlags |= (tr.fraction < 0.50f ? FL_DUCKING : 0) | (tr.fraction != 1 ? FL_ONGROUND : 0);   // Turn flags on
	m_cSoundPlayers[sound.m_nSoundSource - 1].m_nFlags &= (tr.fraction > 0.50f ? ~FL_DUCKING : 0) | (tr.fraction == 1 ? ~FL_ONGROUND : 0); // Turn flags off
}

void CVisuals::RecordInfo()
{
	
}

void CGrenadePrediction::Tick(int buttons)
{
	if (!vars.visuals.nadepred)
		return;
	bool in_attack = buttons & IN_ATTACK;
	bool in_attack2 = buttons & IN_ATTACK2;

	//g_pICvar->Consolecolor_tPrintf(SDK::color_t(255, 255, 255, 255), "OView");

	act = (in_attack && in_attack2) ? ACT_LOB :
		(in_attack2) ? ACT_DROP :
		(in_attack) ? ACT_THROW :
		ACT_NONE;
}
void CGrenadePrediction::View(CViewSetup* setup)
{
	if (!vars.visuals.nadepred)
		return;

	if (csgo->local && csgo->local->isAlive())
	{
		IBaseCombatWeapon* weapon = csgo->local->GetWeapon();

		if (weapon && weapon->IsNade() && act != ACT_NONE)
		{
			type = weapon->GetItemDefinitionIndex();
			Simulate(setup);
		}
		else
		{
			type = 0;
		}
	}
}

void CGrenadePrediction::Paint()
{
	if (!vars.visuals.nadepred)
		return;

	if ((type) && !(path.empty()))
	{
		Vector2D nadeStart, nadeEnd;
		Vector nadeStart1, nadeEnd1;

		Vector prev = path[0];

		for (auto it = path.begin(), end = path.end(); it != end; ++it)
		{
			if (c_renderer::screen_transform(prev, nadeStart, features->Visuals->world_to_screen_matrix) && c_renderer::screen_transform(*it, nadeEnd, features->Visuals->world_to_screen_matrix))
				c_renderer::line(Vector2D(nadeStart.x, nadeStart.y), Vector2D(nadeEnd.x, nadeEnd.y), vars.visuals.nadepred_color);
			
			prev = *it;
		}
		c_renderer::circle_filled(Vector2D(nadeEnd.x, nadeEnd.y), 4, color_t(0, 0, 0, 200));
		c_renderer::circle_filled(Vector2D(nadeEnd.x, nadeEnd.y), 5, color_t(255, 0, 0, 200));
	}
}
const auto PIRAD = 0.01745329251f;
void angle_vectors2(const Vector& angles, Vector* forward, Vector* right, Vector* up)
{
	float sr, sp, sy, cr, cp, cy;

	sp = static_cast<float>(sin(double(angles.x) * PIRAD));
	cp = static_cast<float>(cos(double(angles.x) * PIRAD));
	sy = static_cast<float>(sin(double(angles.y) * PIRAD));
	cy = static_cast<float>(cos(double(angles.y) * PIRAD));
	sr = static_cast<float>(sin(double(angles.z) * PIRAD));
	cr = static_cast<float>(cos(double(angles.z) * PIRAD));

	if (forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right)
	{
		right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right->y = (-1 * sr * sp * sy + -1 * cr * cy);
		right->z = -1 * sr * cp;
	}

	if (up)
	{
		up->x = (cr * sp * cy + -sr * -sy);
		up->y = (cr * sp * sy + -sr * cy);
		up->z = cr * cp;
	}
}
void CGrenadePrediction::Setup(Vector& vecSrc, Vector& vecThrow, Vector viewangles)
{
	if (!vars.visuals.nadepred)
		return;

	Vector angThrow = viewangles;
	float pitch = angThrow.x;

	if (pitch <= 90.0f)
	{
		if (pitch < -90.0f)
		{
			pitch += 360.0f;
		}
	}
	else
	{
		pitch -= 360.0f;
	}
	float a = pitch - (90.0f - fabs(pitch)) * 10.0f / 90.0f;
	angThrow.x = a;

	// Gets ThrowVelocity from weapon files
	// Clamped to [15,750]
	float flVel = 750.0f * 0.9f;

	// Do magic on member of grenade object [esi+9E4h]
	// m1=1  m1+m2=0.5  m2=0
	static const float power[] = { 1.0f, 1.0f, 0.5f, 0.0f };
	float b = power[act];
	// Clamped to [0,1]
	b = b * 0.7f;
	b = b + 0.3f;
	flVel *= b;

	Vector vForward, vRight, vUp;
	angle_vectors2(angThrow, &vForward, &vRight, &vUp); //angThrow.ToVector(vForward, vRight, vUp);

	vecSrc = csgo->unpred_eyepos;
	float off = (power[act] * 12.0f) - 12.0f;
	vecSrc.z += off;

	// Game calls UTIL_TraceHull here with hull and assigns vecSrc tr.endpos
	trace_t tr;
	Vector vecDest = vecSrc;
	vecDest += vForward * 22.0f; //vecDest.MultAdd(vForward, 22.0f);

	TraceHull(vecSrc, vecDest, tr);

	// After the hull trace it moves 6 units back along vForward
	// vecSrc = tr.endpos - vForward * 6
	Vector vecBack = vForward; vecBack *= 6.0f;
	vecSrc = tr.endpos;
	vecSrc -= vecBack;

	// Finally calculate velocity
	vecThrow = csgo->local->GetVelocity(); vecThrow *= 1.25f;
	vecThrow += vForward * flVel; //	vecThrow.MultAdd(vForward, flVel);
}

void CGrenadePrediction::Simulate(CViewSetup* setup)
{
	if (!vars.visuals.nadepred)
		return;
	Vector vecSrc, vecThrow;
	Vector angles; interfaces.engine->GetViewAngles(angles);
	Setup(vecSrc, vecThrow, angles);

	float interval = interfaces.global_vars->interval_per_tick;

	// Log positions 20 times per sec
	int logstep = static_cast<int>(0.05f / interval);
	int logtimer = 0;


	path.clear();
	for (unsigned int i = 0; i < path.max_size() - 1; ++i)
	{
		if (!logtimer)
			path.push_back(vecSrc);

		int s = Step(vecSrc, vecThrow, i, interval);
		if ((s & 1)) break;

		// Reset the log timer every logstep OR we bounced
		if ((s & 2) || logtimer >= logstep) logtimer = 0;
		else ++logtimer;
	}
	path.push_back(vecSrc);
}

int CGrenadePrediction::Step(Vector& vecSrc, Vector& vecThrow, int tick, float interval)
{

	// Apply gravity
	Vector move;
	AddGravityMove(move, vecThrow, interval, false);

	// Push entity
	trace_t tr;
	PushEntity(vecSrc, move, tr);

	int result = 0;
	// Check ending conditions
	if (CheckDetonate(vecThrow, tr, tick, interval))
	{
		result |= 1;
	}

	// Resolve collisions
	if (tr.fraction != 1.0f)
	{
		result |= 2; // Collision!
		ResolveFlyCollisionCustom(tr, vecThrow, interval);
	}

	// Set new position
	vecSrc = tr.endpos;

	return result;
}


bool CGrenadePrediction::CheckDetonate(const Vector& vecThrow, const trace_t& tr, int tick, float interval)
{
	switch (type)
	{
	case WEAPON_SMOKEGRENADE:
	case WEAPON_DECOY:
		// Velocity must be <0.1, this is only checked every 0.2s
		if (vecThrow.Length2D() < 0.1f)
		{
			int det_tick_mod = static_cast<int>(0.2f / interval);
			return !(tick % det_tick_mod);
		}
		return false;

	case WEAPON_MOLOTOV:
	case WEAPON_INC:
		// Detonate when hitting the floor
		if (tr.fraction != 1.0f && tr.plane.normal.z > 0.7f)
			return true;
		// OR we've been flying for too long

	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
		// Pure timer based, detonate at 1.5s, checked every 0.2s
		return static_cast<float>(tick) * interval > 1.5f && !(tick % static_cast<int>(0.2f / interval));

	default:
		assert(false);
		return false;
	}
}

void CGrenadePrediction::TraceHull(Vector& src, Vector& end, trace_t& tr)
{
	if (!vars.visuals.nadepred)
		return;
	Ray_t ray;
	ray.Init(src, end, Vector(-2.0f, -2.0f, -2.0f), Vector(2.0f, 2.0f, 2.0f));

	CTraceFilterWorldAndPropsOnly filter;
	//filter.SetIgnoreClass("BaseCSGrenadeProjectile");
	//filter.bShouldHitPlayers = false;

	interfaces.trace->TraceRay(ray, 0x200400B, &filter, &tr);
}

void CGrenadePrediction::AddGravityMove(Vector& move, Vector& vel, float frametime, bool onground)
{
	if (!vars.visuals.nadepred)
		return;
	Vector basevel(0.0f, 0.0f, 0.0f);

	move.x = (vel.x + basevel.x) * frametime;
	move.y = (vel.y + basevel.y) * frametime;

	if (onground)
	{
		move.z = (vel.z + basevel.z) * frametime;
	}
	else
	{
		// Game calls GetActualGravity( this );
		float gravity = 800.0f * 0.4f;

		float newZ = vel.z - (gravity * frametime);
		move.z = ((vel.z + newZ) / 2.0f + basevel.z) * frametime;

		vel.z = newZ;
	}
}

void CGrenadePrediction::PushEntity(Vector& src, const Vector& move, trace_t& tr)
{
	if (!vars.visuals.nadepred)
		return;
	Vector vecAbsEnd = src;
	vecAbsEnd += move;

	// Trace through world
	TraceHull(src, vecAbsEnd, tr);
}

void CGrenadePrediction::ResolveFlyCollisionCustom(trace_t& tr, Vector& vecVelocity, float interval)
{
	if (!vars.visuals.nadepred)
		return;
	// Calculate elasticity
	float flSurfaceElasticity = 1.0;  // Assume all surfaces have the same elasticity
	float flGrenadeElasticity = 0.45f; // GetGrenadeElasticity()
	float flTotalElasticity = flGrenadeElasticity * flSurfaceElasticity;
	if (flTotalElasticity > 0.9f) flTotalElasticity = 0.9f;
	if (flTotalElasticity < 0.0f) flTotalElasticity = 0.0f;

	// Calculate bounce
	Vector vecAbsVelocity;
	PhysicsClipVelocity(vecVelocity, tr.plane.normal, vecAbsVelocity, 2.0f);
	vecAbsVelocity *= flTotalElasticity;

	// Stop completely once we move too slow
	float flSpeedSqr = vecAbsVelocity.LengthSqr();
	static const float flMinSpeedSqr = 20.0f * 20.0f; // 30.0f * 30.0f in CSS
	if (flSpeedSqr < flMinSpeedSqr)
	{
		//vecAbsVelocity.Zero();
		vecAbsVelocity.x = 0.0f;
		vecAbsVelocity.y = 0.0f;
		vecAbsVelocity.z = 0.0f;
	}

	// Stop if on ground
	if (tr.plane.normal.z > 0.7f)
	{
		vecVelocity = vecAbsVelocity;
		vecAbsVelocity *= ((1.0f - tr.fraction) * interval); //vecAbsVelocity.Mult((1.0f - tr.fraction) * interval);
		PushEntity(tr.endpos, vecAbsVelocity, tr);
	}
	else
	{
		vecVelocity = vecAbsVelocity;
	}
}

int CGrenadePrediction::PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce)
{
	static const float STOP_EPSILON = 0.1f;

	float    backoff;
	float    change;
	float    angle;
	int        i, blocked;

	blocked = 0;

	angle = normal[2];

	if (angle > 0)
	{
		blocked |= 1;        // floor
	}
	if (!angle)
	{
		blocked |= 2;        // step
	}

	backoff = in.Dot(normal) * overbounce;

	for (i = 0; i < 3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
		{
			out[i] = 0;
		}
	}

	return blocked;
}

void CVisuals::NightMode()
{
	static auto load_named_sky = reinterpret_cast<void(__fastcall*)(const char*)>(
		csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str()),
			hs::load_named_sky::s().c_str()));

	static auto sv_skyname = interfaces.cvars->FindVar(hs::sv_skyname::s().c_str());
	sv_skyname->m_fnChangeCallbacks.m_Size = 0;

	static auto r_3dsky = interfaces.cvars->FindVar(hs::r_3dsky::s().c_str());
	r_3dsky->m_fnChangeCallbacks.m_Size = 0;
	r_3dsky->SetValue(0);

	static auto backup = sv_skyname->GetString();

	const auto reset = [&]()
	{
		load_named_sky(hs::sky_dust::s().c_str());

		for (auto i = interfaces.material_system->FirstMaterial(); i != interfaces.material_system->InvalidMaterial(); i = interfaces.material_system->NextMaterial(i))
		{
			auto mat = interfaces.material_system->GetMaterial(i);
			if (!mat)
				continue;

			if (mat->IsErrorMaterial())
				continue;

			std::string name = mat->GetName();
			auto tex_name = mat->GetTextureGroupName();


			if (strstr(tex_name, hs::World::s().c_str()) || strstr(tex_name, hs::StaticProp::s().c_str()) || strstr(tex_name, hs::SkyBox::s().c_str()))
			{
				mat->ColorModulate(1.f, 1.f, 1.f);
				mat->AlphaModulate(1.f);
			}
		}
	};

	static bool Reset = false;
	static bool Reset2 = false;

	if (vars.visuals.nightmode)
	{
		Reset = false;

		load_named_sky(hs::sky_csgo_night02::s().c_str());

		static float OldClr[3];
		static float OldClr1[3];
		static float OldClr2[3];

		if (csgo->mapChanged || !Reset2 || OldClr[0] != vars.visuals.nightmode_color[0]
			|| OldClr[1] != vars.visuals.nightmode_color[1]
			|| OldClr[2] != vars.visuals.nightmode_color[2])
		{
			for (auto i = interfaces.material_system->FirstMaterial(); i != interfaces.material_system->InvalidMaterial(); i = interfaces.material_system->NextMaterial(i))
			{
				auto mat = interfaces.material_system->GetMaterial(i);
				if (!mat)
					continue;

				auto tex_name = mat->GetTextureGroupName();

				if (strstr(tex_name, hs::World::s().c_str()))
				{
					mat->ColorModulate(vars.visuals.nightmode_color[0] / 255.f,
						vars.visuals.nightmode_color[1] / 255.f, vars.visuals.nightmode_color[2] / 255.f);
				}
			}

			OldClr[0] = vars.visuals.nightmode_color[0];
			OldClr[1] = vars.visuals.nightmode_color[1];
			OldClr[2] = vars.visuals.nightmode_color[2];
			csgo->mapChanged = false;
			Reset2 = true;
		}

		if (csgo->mapChanged || !Reset2 || OldClr1[0] != vars.visuals.nightmode_prop_color[0]
			|| OldClr1[1] != vars.visuals.nightmode_prop_color[1]
			|| OldClr1[2] != vars.visuals.nightmode_prop_color[2])
		{
			for (auto i = interfaces.material_system->FirstMaterial(); i != interfaces.material_system->InvalidMaterial(); i = interfaces.material_system->NextMaterial(i))
			{
				auto mat = interfaces.material_system->GetMaterial(i);
				if (!mat)
					continue;

				auto tex_name = mat->GetTextureGroupName();

				if (strstr(tex_name, hs::StaticProp::s().c_str()))
				{
					mat->ColorModulate(vars.visuals.nightmode_prop_color[0] / 255.f,
						vars.visuals.nightmode_prop_color[1] / 255.f, vars.visuals.nightmode_prop_color[2] / 255.f);
				}
			}

			OldClr1[0] = vars.visuals.nightmode_prop_color[0];
			OldClr1[1] = vars.visuals.nightmode_prop_color[1];
			OldClr1[2] = vars.visuals.nightmode_prop_color[2];
			csgo->mapChanged = false;
			Reset2 = true;
		}

		if (csgo->mapChanged || !Reset2 || OldClr2[0] != vars.visuals.nightmode_skybox_color[0]
			|| OldClr2[1] != vars.visuals.nightmode_skybox_color[1]
			|| OldClr2[2] != vars.visuals.nightmode_skybox_color[2])
		{
			for (auto i = interfaces.material_system->FirstMaterial(); i != interfaces.material_system->InvalidMaterial(); i = interfaces.material_system->NextMaterial(i))
			{
				auto mat = interfaces.material_system->GetMaterial(i);
				if (!mat)
					continue;

				auto tex_name = mat->GetTextureGroupName();

				if (strstr(tex_name, hs::SkyBox::s().c_str()))
				{
					mat->ColorModulate(vars.visuals.nightmode_skybox_color[0] / 255.f,
						vars.visuals.nightmode_skybox_color[1] / 255.f, vars.visuals.nightmode_skybox_color[2] / 255.f);
				}
			}

			OldClr2[0] = vars.visuals.nightmode_skybox_color[0];
			OldClr2[1] = vars.visuals.nightmode_skybox_color[1];
			OldClr2[2] = vars.visuals.nightmode_skybox_color[2];
			csgo->mapChanged = false;
			Reset2 = true;
		}
	}
	else
	{
		Reset2 = false;

		if (!Reset)
		{
			reset();
			Reset = true;
		}
		csgo->mapChanged = false;
	}
}