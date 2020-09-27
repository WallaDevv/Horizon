#include "Menu.h"
#include "GUI/gui.h"
#include "shonax things/parser/dyn_shonax.h"

#include <unordered_map>
#include <set>
#include <algorithm>





#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#define FCVAR_HIDDEN			(1<<4)	// Hidden. Doesn't appear in find or 
#define FCVAR_UNREGISTERED		(1<<0)	// If this is set, don't add to linked list, etc.
#define FCVAR_DEVELOPMENTONLY	(1<<1)	// Hidden in released products. Flag is removed 

vector<string> ConfigList;
typedef void(*LPSEARCHFUNC)(LPCTSTR lpszFileName);

int fix_item_def_knifes()
{

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
}


void c_menu::keybind_list()
{
	auto color = vars.misc.watermark_color;
	int size = 0;

	if (vars.ragebot.double_tap->active) {
		size = size + 11;
		switch (vars.ragebot.double_tap->type)
		{
		case 0:
			break;
		case 1:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[holding]");
			break;
		case 2:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[toggled]");
			break;
		case 3:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[release]");
			break;
		case 4:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[always]");
			break;
		}
		g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "   doubletap");
	}

	if (vars.antiaim.inverter->active) {
		size = size + 11;
		switch (vars.antiaim.inverter->type)
		{
		case 0:
			break;
		case 1:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[holding]");
			break;
		case 2:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[toggled]");
			break;
		case 3:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[release]");
			break;
		case 4:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[always]");
			break;
		}
		g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "   inverter");
	}

	if (vars.antiaim.fakeduck->active) {
		size = size + 11;
		switch (vars.antiaim.fakeduck->type)
		{
		case 0:
			break;
		case 1:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[holding]");
			break;
		case 2:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[toggled]");
			break;
		case 3:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[release]");
			break;
		case 4:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[always]");
			break;
		}
		g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "   fakeduck");
	}

	if (vars.antiaim.slowwalk->active) {
		size = size + 11;
		switch (vars.antiaim.slowwalk->type)
		{
		case 0:
			break;
		case 1:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[holding]");
			break;
		case 2:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[toggled]");
			break;
		case 3:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[release]");
			break;
		case 4:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[always]");
			break;
		}
		g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "   slowwalk");
	}

	if (vars.misc.thirdperson_bind->active) {
		size = size + 11;
		switch (vars.misc.thirdperson_bind->type)
		{
		case 0:
			break;
		case 1:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[holding]");
			break;
		case 2:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[toggled]");
			break;
		case 3:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[release]");
			break;
		case 4:
			g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "								[Always]");
			break;
		}
		g_Render->DrawString(vars.misc.keybind_list_x, vars.misc.keybind_list_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "   thirdperson");
	}



	g_Render->FilledRect(vars.misc.keybind_list_x, vars.misc.keybind_list_y, 150, 30 + size, color_t(15, 15, 15, 100));
	g_Render->Rect(vars.misc.keybind_list_x, vars.misc.keybind_list_y, 150, 1, color);

	g_Render->DrawString(vars.misc.keybind_list_x + 25, vars.misc.keybind_list_y + 5, color, render::outline, fonts::menu_main, "   Active Keybinds");
}

void c_menu::spec_list()
{
	auto color = vars.misc.watermark_color;
	int size = 0;

	for (int i = 1; i <= interfaces.global_vars->maxClients; i++)
	{
		auto pEntity = interfaces.ent_list->GetClientEntity(i);
		player_info_t pinfo;

		if (pEntity && pEntity != csgo->local)
		{
			if (interfaces.engine->GetPlayerInfo(i, &pinfo) && !pEntity->isAlive() && !pEntity->IsDormant())
			{
				HANDLE obs = pEntity->GetObserverTargetHandle();

				if (obs)
				{
					auto pTarget = interfaces.ent_list->GetClientEntityFromHandle((uint32_t)obs);
					player_info_t pinfo2;
					if (pTarget)
					{
						if (interfaces.engine->GetPlayerInfo(pTarget->GetIndex(), &pinfo2))
						{
							if (pTarget->GetIndex() == csgo->local->GetIndex())
							{
								size = size + 15;
								g_Render->DrawString(vars.misc.speclist_x + 10, vars.misc.speclist_y + 10 + size, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "   %s", pinfo.name);
							}
						}
					}
				}
			}
		}
	}

	g_Render->FilledRect(vars.misc.speclist_x, vars.misc.speclist_y, 200, 30 + size, color_t(15, 15, 15, 100));
	g_Render->Rect(vars.misc.speclist_x, vars.misc.speclist_y, 200, 1, color);

	g_Render->DrawString(vars.misc.speclist_x + 10, vars.misc.speclist_y + 10, color, render::outline, fonts::menu_main, "   Spectator list");
}

void c_menu::monitor()
{
	auto color = vars.misc.watermark_color;

	g_Render->FilledRect(vars.misc.monitor_x, vars.misc.monitor_y, 75, 22, color_t(15, 15, 15, 100));
	g_Render->Rect(vars.misc.monitor_x, vars.misc.monitor_y, 75, 1, color);

	g_Render->DrawString(vars.misc.monitor_x + 1, vars.misc.monitor_y + 1, color, render::outline, fonts::menu_main, "fake:\t%.1f", csgo->desync_angle);

	g_Render->DrawString(vars.misc.monitor_x + 1, vars.misc.monitor_y + 11, color, render::outline, fonts::menu_main, "choke:\t%.1f", csgo->chockedticks);
}



BOOL SearchFiles(LPCTSTR lpszFileName, LPSEARCHFUNC lpSearchFunc, BOOL bInnerFolders = FALSE)
{
	LPTSTR part;
	char tmp[MAX_PATH];
	char name[MAX_PATH];

	HANDLE hSearch = NULL;
	WIN32_FIND_DATA wfd;
	memset(&wfd, 0, sizeof(WIN32_FIND_DATA));

	if (bInnerFolders)
	{
		if (GetFullPathName(lpszFileName, MAX_PATH, tmp, &part) == 0) return FALSE;
		strcpy(name, part);
		strcpy(part, "*.*");
		wfd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
		if (!((hSearch = FindFirstFile(tmp, &wfd)) == INVALID_HANDLE_VALUE))
			do
			{
				if (!strncmp(wfd.cFileName, ".", 1) || !strncmp(wfd.cFileName, "..", 2))
					continue;

				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					char next[MAX_PATH];
					if (GetFullPathName(lpszFileName, MAX_PATH, next, &part) == 0) return FALSE;
					strcpy(part, wfd.cFileName);
					strcat(next, "\\Horizon\\*.json");
					strcat(next, name);

					SearchFiles(next, lpSearchFunc, TRUE);
				}
			} while (FindNextFile(hSearch, &wfd));
			FindClose(hSearch);
	}

	if ((hSearch = FindFirstFile(lpszFileName, &wfd)) == INVALID_HANDLE_VALUE)
		return TRUE;
	do
		if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			char file[MAX_PATH];
			if (GetFullPathName(lpszFileName, MAX_PATH, file, &part) == 0) return FALSE;
			strcpy(part, wfd.cFileName);

			lpSearchFunc(wfd.cFileName);
		}
	while (FindNextFile(hSearch, &wfd));
	FindClose(hSearch);
	return TRUE;
}
void ReadConfigs(LPCTSTR lpszFileName)
{
	ConfigList.push_back(lpszFileName);
}

void RefreshConfigs()
{
	static TCHAR path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		ConfigList.clear();
		string ConfigDir = std::string(path) + "\\Horizon\\*.json";
		SearchFiles(ConfigDir.c_str(), ReadConfigs, FALSE);
	}

}
void EnableHiddenCVars()
{
	auto p = **reinterpret_cast<ConCommandBase***>(interfaces.cvars + 0x34);
	for (auto c = p->m_pNext; c != nullptr; c = c->m_pNext)
	{
		ConCommandBase* cmd = c;
		cmd->m_nFlags &= ~FCVAR_DEVELOPMENTONLY;
		cmd->m_nFlags &= ~FCVAR_HIDDEN;
	}
}

Vector2D g_mouse;
//color_t main_color = color_t(255, 150, 0);
color_t main_color = color_t(255, 0, 0);

bool enable_legit() { return vars.legitbot.legitenable; };
bool enable_rage() { return vars.ragebot.enable; };
bool enable_antiaim() { return vars.antiaim.enable; };
bool enable_esp() { return vars.visuals.enable; };

bool need_warning() {
	return (csgo->expire_date - time(0)) / 3600 < 48;
};
int getfps()
{
	return static_cast<int>(1.f / interfaces.global_vars->frametime);
}
bool IsInGame()
{
	return interfaces.engine->IsConnected() && interfaces.engine->IsInGame();
}
int getping()
{
	if (!IsInGame())
		return 0;

	auto nci = interfaces.engine->GetNetChannelInfo();
	if (!nci)
		return 0;

	float m_AvgLatency = nci->GetAvgLatency(FLOW_INCOMING);

	return (int)(m_AvgLatency * 1000.0f);

}

std::string get_date()
{
	auto time = std::time(nullptr);
	auto time_s = std::tm();

	std::ostringstream stream;

	if (localtime_s(&time_s, &time) == 0)
		stream << std::put_time(&time_s, "%H:%M:%S %d.%m.%Y"); // 12:05:2 26.07.2017

	return stream.str();
}

void c_menu::watermark()
{
	int x, y;
	interfaces.engine->GetScreenSize(x, y);

	std::time_t result = std::time(nullptr);

	auto color = vars.misc.watermark_color;

	g_Render->FilledRect(x - 295, 4, 285, 20, color_t(15, 15, 15, 155));
	g_Render->Rect(x - 295, 4, 285, 1, color);

	//g_Render->DrawString(x - 290, 8, color_t(255, 255, 255, 255), render::outline, fonts::menu_main, "Loneless");
	g_Render->DrawString(x - 292, 8, color_t(255, 255, 255, 130), render::outline, fonts::menu_main, "Horizon");
	g_Render->DrawString(x - 239, 8, color_t(255, 255, 255, 130),  render::outline, fonts::menu_main, " | Beta");
	g_Render->DrawString(x - 201, 8, color_t(255, 255, 255, 130), render::outline, fonts::menu_main, (" | Fps: %03d"), getfps());
	g_Render->DrawString(x - 146, 8, color_t(255, 255, 255, 130), render::outline, fonts::menu_main, (" | Ping: %03d"), getping());
	g_Render->DrawString(x - 86, 8, color_t(255, 255, 255, 130), render::outline, fonts::menu_main,   " | " __TIME__);
}


Vector2D get_mouse_pos()
{
	POINT mousePosition;
	GetCursorPos(&mousePosition);
	ScreenToClient(FindWindow(0, "Counter-Strike: Global Offensive"), &mousePosition);
	return { static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y) };
}
bool mouse_in_region(int x, int y, int x2, int y2)
{
	if (get_mouse_pos().x > x&& get_mouse_pos().y > y&& get_mouse_pos().x < x2 + x && get_mouse_pos().y < y2 + y)
		return true;

	return false;
}
void c_menu::render() {
	



	static Vector2D _mouse_pos;
	static int groupbox_bottom;
	static int dragX = 0;
	static int dragY = 0;
	static int Width = 538;
	static int Height = 600;
	static Vector2D oldPos;
	static Vector2D mousePos;
	static bool Dragging = false;
	bool click = false;
	static int iScreenWidth, iScreenHeight;
	static bool _pressed = true;



	if (window) {
		window->update_keystates();
		window->update_animation();
		update_binds();

		if (GetAsyncKeyState(VK_LBUTTON))
			click = true;

		Vector2D MousePos = get_mouse_pos();

		if (Dragging && !click)
			Dragging = false;

		if (Dragging && click)
		{
			window->set_position_x(MousePos.x - dragX);
			window->set_position_y(MousePos.y - dragY);
		}
		if (mouse_in_region(window->get_position().x, window->get_position().y, Width, 20))
		{
			Dragging = true;
			dragX = MousePos.x - window->get_position().x;
			dragY = MousePos.y - window->get_position().y;
		}
		if (window->get_position().x < 0)
			window->set_position_x(0);
		if (window->get_position().y < 0)
			window->set_position_y(0);
		if ((window->get_position().x + Width) > iScreenWidth)
			window->set_position_x(iScreenWidth - Width);
		if ((window->get_position().y + Height) > iScreenHeight)
			window->set_position_y(iScreenHeight - Height);
	}



	static bool once = false;
	if (!once) {
		Config.ResetToDefault();
		vars.menu.open = true;
		once = true;
	}
	interfaces.engine->GetScreenSize(iScreenWidth, iScreenHeight);
	if (initialized) {
		if (window->get_transparency() < 100.f && vars.menu.open)
			window->increase_transparency(animation_speed * 40.f);
	}

	else {

		window = new c_window();
		window->set_size(Vector2D(538, 600));
		window->set_position(Vector2D(10, 10));

		window->add_tab(new c_tab("ragebot", tab_t::rage, window)); {
			auto main_child = new c_child("main", tab_t::rage, window);
			main_child->set_size(Vector2D(250, 255));
			main_child->set_position(Vector2D(0, 0)); {

				main_child->add_element(new c_checkbox("enable", 
					&vars.ragebot.enable));

				/*main_child->add_element(new c_checkbox("aimstep", &vars.misc.aim_step,
					enable_rage));*/

				main_child->add_element(new c_checkbox("silent aim",
					&vars.ragebot.silentaim, enable_rage));

				main_child->add_element(new c_checkbox("auto shot", 
					&vars.ragebot.autoshoot, enable_rage));

				main_child->add_element(new c_checkbox("auto scope", 
					&vars.ragebot.autoscope,  enable_rage));

				main_child->add_element(new c_checkbox("backtrack", 
					&vars.ragebot.posadj, enable_rage));

				main_child->add_element(new c_checkbox("on shot priority", &vars.ragebot.backshoot_bt,
					[]() { return vars.ragebot.enable && vars.ragebot.posadj; }));

				main_child->add_element(new c_checkbox("delay shot", &vars.ragebot.delayshot,
					enable_rage));

				main_child->add_element(new c_checkbox("resolver", &vars.ragebot.resolver,
					enable_rage));

				main_child->add_element(new c_keybind("force baim", vars.ragebot.force_body, []() {
					return vars.ragebot.enable;
				}));
				
				main_child->add_element(new c_keybind("double tap", vars.ragebot.double_tap, []() { 
					return vars.ragebot.enable;
				}));
				 
				main_child->add_element(new c_checkbox("instant double tap",
					&vars.ragebot.dt_instant, []() { return vars.ragebot.double_tap->key > 0; }));

				main_child->add_element(new c_checkbox("disable double tap delay",
					&vars.ragebot.disable_dt_delay, []() { return vars.ragebot.double_tap->key > 0; }));

				main_child->initialize_elements();
			}
			window->add_element(main_child);

			auto antiaim_main = new c_child("anti-aim", tab_t::rage, window);
			antiaim_main->set_size(Vector2D(250, 255));
			antiaim_main->set_position(Vector2D(0, 265)); {

				antiaim_main->add_element(new c_checkbox("enable", &vars.antiaim.enable));
				antiaim_main->add_element(new c_slider("desync amount", &vars.antiaim.desync_amount, 0.f, 100.f, "%0.f%%", enable_antiaim));


				antiaim_main->add_element(new c_combo("Pitch", &vars.antiaim.pitch,
					{
						"Down",
						"Zero",
						"Up",
						"Fake Pitch",
					},
					enable_antiaim));

				antiaim_main->add_element(new c_keybind("Inverter", vars.antiaim.inverter,
					[]() { return enable_antiaim(); }));

				antiaim_main->add_element(new c_combo("desync type",
					&vars.antiaim.desync_type, {
						"static",
						"balance",
						"jitter",
						"jitter switch",
						"jitter random"
					}, enable_antiaim));


				antiaim_main->add_element(new c_slider("", &vars.antiaim.jitterrange, 0, 180, "%.0f jitter range",
					[]() { return enable_antiaim(); }));

				

				antiaim_main->add_element(new c_checkbox("manunal override",
					&vars.antiaim.aa_override.enable, []() { return enable_antiaim(); }));

				antiaim_main->add_element(new c_keybind("left", vars.antiaim.aa_override.left,
					[]() { return enable_antiaim() && vars.antiaim.aa_override.enable; }));
				antiaim_main->add_element(new c_keybind("right", vars.antiaim.aa_override.right,
					[]() { return enable_antiaim() && vars.antiaim.aa_override.enable; }));
				antiaim_main->add_element(new c_keybind("back", vars.antiaim.aa_override.back,
					[]() { return enable_antiaim() && vars.antiaim.aa_override.enable; }));

				antiaim_main->add_element(new c_combo("fake lag", &vars.antiaim.fakelag,
					{
						"off",
						"factor",
						"fluctuate",
					},
					enable_antiaim));

				antiaim_main->add_element(new c_slider("", &vars.antiaim.fakelagfactor, 1, 14, "%.0f packets",
					[]() { return enable_antiaim() && vars.antiaim.fakelag > 0; }));

				antiaim_main->add_element(new c_slider("", &vars.antiaim.fakelagvariance, 0.f, 100.f, "%.0f ticks to switch",
					[]() { return enable_antiaim() && vars.antiaim.fakelag == 2; }));

				antiaim_main->add_element(new c_checkbox("teleport exploit",
					&vars.antiaim.fakelag_onshot, []() { return enable_antiaim() && vars.antiaim.fakelag; }));

				antiaim_main->add_element(new c_keybind("fake duck", vars.antiaim.fakeduck,
					[]() { return enable_antiaim(); }));

				antiaim_main->add_element(new c_keybind("slow walk", vars.antiaim.slowwalk,
					[]() { return enable_antiaim(); }));

				/*antiaim_main->add_element(new c_slider("", &vars.antiaim.slowwalk_speed, 10, 90, "%.0f%%",
					[]() { return enable_antiaim(); }));*/

				antiaim_main->initialize_elements();
			}
			window->add_element(antiaim_main);

			reinit_weapon_cfg();
		}
		window->set_transparency(100.f);
		window->add_tab(new c_tab("players", tab_t::esp, window)); {
			reinit_chams();

			auto esp_main = new c_child("esp", tab_t::esp, window);
			esp_main->set_size(Vector2D(250, 255));
			esp_main->set_position(Vector2D(258, 0)); {

				esp_main->add_element(new c_checkbox("enable",
					&vars.visuals.enable));

				esp_main->add_element(new c_checkbox("on dormant",
					&vars.visuals.dormant, enable_esp));

				esp_main->add_element(new c_colorpicker(&vars.visuals.box_color,
					color_t(255, 255, 255, 255), []() { return enable_esp() && vars.visuals.box; }));

				esp_main->add_element(new c_checkbox("box",
					&vars.visuals.box, enable_esp));

				esp_main->add_element(new c_colorpicker(&vars.visuals.skeleton_color,
					color_t(255, 255, 255, 255), []() { return enable_esp() && vars.visuals.skeleton; }));

				esp_main->add_element(new c_checkbox("skeleton",
					&vars.visuals.skeleton, enable_esp));

				esp_main->add_element(new c_checkbox("health",
					&vars.visuals.healthbar, enable_esp));

				esp_main->add_element(new c_colorpicker(&vars.visuals.hp_color,
					color_t(255, 255, 255, 255), []() { return enable_esp() && vars.visuals.override_hp; }));

				esp_main->add_element(new c_checkbox("override health",
					&vars.visuals.override_hp, [] { return enable_esp() && vars.visuals.healthbar; }));

				esp_main->add_element(new c_colorpicker(&vars.visuals.name_color,
					color_t(255, 255, 255, 255), []() { return enable_esp() && vars.visuals.name; }));

				esp_main->add_element(new c_checkbox("name",
					&vars.visuals.name, enable_esp));

				esp_main->add_element(new c_colorpicker(&vars.visuals.weapon_color,
					color_t(255, 255, 255, 255), []() { return enable_esp() && vars.visuals.weapon; }));

				esp_main->add_element(new c_checkbox("weapon",
					&vars.visuals.weapon, enable_esp));

				esp_main->add_element(new c_colorpicker(&vars.visuals.ammo_color,
					color_t(255, 255, 255, 255), []() { return enable_esp() && vars.visuals.ammo; }));

				esp_main->add_element(new c_checkbox("ammo",
					&vars.visuals.ammo, enable_esp));

				esp_main->add_element(new c_colorpicker(&vars.visuals.flags_color,
					color_t(255, 255, 255, 255), []() { return enable_esp() && vars.visuals.flags > 0; }));

				esp_main->add_element(new c_multicombo("flags",
					&vars.visuals.flags, {
						"armor",
						"scoped",
						"flashed",
						"can hit",
						"resolver mode",
						"choke count"
					}, enable_esp));
				esp_main->add_element(new c_checkbox("show multipoint", &vars.visuals.shot_multipoint, enable_esp));

				esp_main->add_element(new c_colorpicker(&vars.visuals.out_of_fov_color,
					color_t(255, 255, 255, 255), []() { return enable_esp() && vars.visuals.out_of_fov; }));

				esp_main->add_element(new c_checkbox("out of fov arrow",
					&vars.visuals.out_of_fov, enable_esp));

				esp_main->add_element(new c_slider("size", &vars.visuals.out_of_fov_size, 10, 30, "%.0f px",
					[]() { return enable_esp() && vars.visuals.out_of_fov; }));
				esp_main->add_element(new c_slider("distance", &vars.visuals.out_of_fov_distance, 5, 30, "%.0f",
					[]() { return enable_esp() && vars.visuals.out_of_fov; }));
				esp_main->initialize_elements();
			}
			window->add_element(esp_main);

			auto misc_esp_main = new c_child("misc", tab_t::esp, window);
			misc_esp_main->set_size(Vector2D(250, 255));
			misc_esp_main->set_position(Vector2D(258, 265)); {

				misc_esp_main->add_element(new c_colorpicker(&vars.visuals.hitmarker_color,
					color_t(255, 255, 255, 255), []() { return vars.visuals.hitmarker > 0; }));

				misc_esp_main->add_element(new c_combo("hitmarker",
					&vars.visuals.hitmarker, {"off", "circle", "cross"}));

				misc_esp_main->add_element(new c_checkbox("hitmarker sound",
					&vars.visuals.hitmarker_sound, []() { return vars.visuals.hitmarker > 0; }));

				misc_esp_main->add_element(new c_colorpicker(&vars.visuals.eventlog_color,
					color_t(255, 255, 255, 255), []() { return vars.visuals.eventlog; }));

				misc_esp_main->add_element(new c_checkbox("event log",
					&vars.visuals.eventlog));

				misc_esp_main->add_element(new c_multicombo("indicator",
					&vars.visuals.indicators, {
						"Desync",
						"lag comp",	
						"manual antiaim"
					}));

				misc_esp_main->add_element(new c_checkbox("thirdperson", 
					&vars.misc.thirdperson, nullptr, vars.misc.thirdperson_bind));

				misc_esp_main->add_element(new c_slider("", &vars.visuals.thirdperson_dist, 0, 300, "%.0f units"));

				misc_esp_main->add_element(new c_slider("aspect ratio", &vars.visuals.aspect_ratio, 0, 250, "%.0f"));
				misc_esp_main->initialize_elements();
			}
			window->add_element(misc_esp_main);
		}

		window->add_tab(new c_tab("world", tab_t::world, window)); {

			auto trace_child = new c_child("tracers", tab_t::world, window);
			trace_child->set_size(Vector2D(250, 520));
			trace_child->set_position(Vector2D(0, 0)); {
				trace_child->add_element(new c_colorpicker(&vars.visuals.bullet_tracer_color,
					color_t(255, 255, 255, 255),  []() { return vars.visuals.bullet_tracer; }));

				trace_child->add_element(new c_checkbox("bullet tracer",
					&vars.visuals.bullet_tracer));

				trace_child->add_element(new c_colorpicker(&vars.visuals.bullet_tracer_local_color,
					color_t(255, 255, 255, 255),  []() { return vars.visuals.bullet_tracer_local; }));

				trace_child->add_element(new c_checkbox("local bullet tracer",
					&vars.visuals.bullet_tracer_local));

				trace_child->add_element(new c_combo("sprite", &vars.visuals.bullet_tracer_type,
				{
					"default",
					"phys beam",
					"bubble",
					"glow"
				},
				 []() { return vars.visuals.bullet_tracer; }));

				trace_child->add_element(new c_checkbox("show impacts",
					&vars.visuals.bullet_impact));

				trace_child->add_element(new c_colorpicker(&vars.visuals.bullet_impact_color,
					color_t(255, 0, 0, 255),  []() { return vars.visuals.bullet_impact; }));

				trace_child->add_element(new c_text("server impact",  []() { return vars.visuals.bullet_impact; }));

				trace_child->add_element(new c_colorpicker(&vars.visuals.client_impact_color,
					color_t(255, 0, 0, 255),  []() { return vars.visuals.bullet_impact; }));

				trace_child->add_element(new c_text("client impact",  []() { return vars.visuals.bullet_impact; }));

				trace_child->add_element(new c_slider("impacts size", &vars.visuals.impacts_size, 0.f, 5.f, "%.2f%%"));

				trace_child->add_element(new c_colorpicker(&vars.visuals.nadepred_color,
					color_t(255, 255, 255, 255),  []() { return vars.visuals.nadepred; }));

				trace_child->add_element(new c_checkbox("grenade tracer",
					&vars.visuals.nadepred));
				trace_child->initialize_elements();
			}
			window->add_element(trace_child);

			auto effects_child = new c_child("effects", tab_t::world, window);
			effects_child->set_size(Vector2D(250, 520));
			effects_child->set_position(Vector2D(258, 0)); {

				effects_child->add_element(new c_multicombo("removals",
					&vars.visuals.remove, {
						"visual recoil",
						"smoke",
						"flash",
						"scope",
						"zoom",
						"post processing",
						"fog",
						"shadow"
					}));

				effects_child->add_element(new c_checkbox("night mode", &vars.visuals.nightmode));
				{
					effects_child->add_element(new c_colorpicker(&vars.visuals.nightmode_color,
						color_t(101, 97, 107, 255),  []() { return vars.visuals.nightmode; }));

					effects_child->add_element(new c_text("world",  []() { return vars.visuals.nightmode; }));


					effects_child->add_element(new c_colorpicker(&vars.visuals.nightmode_prop_color,
						color_t(255, 255, 255, 255),  []() { return vars.visuals.nightmode; }));

					effects_child->add_element(new c_text("props",  []() { return vars.visuals.nightmode; }));


					effects_child->add_element(new c_colorpicker(&vars.visuals.nightmode_skybox_color,
						color_t(194, 101, 35, 255),  []() { return vars.visuals.nightmode; }));

					effects_child->add_element(new c_text("skybox",  []() { return vars.visuals.nightmode; }));
				}

				effects_child->add_element(new c_checkbox("force crosshair",
					&vars.visuals.force_crosshair));

				effects_child->add_element(new c_checkbox("kill fade",
					&vars.visuals.kill_effect));

				effects_child->add_element(new c_slider("world fov", &vars.misc.worldfov, 90, 145, "%.0f"));

				effects_child->add_element(new c_slider("viewmodel fov", &vars.misc.viewmodelfov, 68, 145, "%.0f"));
				effects_child->initialize_elements();
			}
			window->add_element(effects_child);
		}

		window->add_tab(new c_tab("misc", tab_t::misc, window)); {
			auto main_child = new c_child("main", tab_t::misc, window);
			main_child->set_size(Vector2D(250, 255));
			main_child->set_position(Vector2D(0, 0)); {

				main_child->add_element(new c_checkbox("anti untrusted",
					&vars.misc.antiuntrusted));

				main_child->add_element(new c_checkbox("bunny hop",
					&vars.misc.bunnyhop));

				main_child->add_element(new c_checkbox("knife-bot",
					&vars.misc.knifebot));

				main_child->add_element(new c_checkbox("horizon clan-tag",
					&vars.visuals.clantagspammer));
			
				main_child->add_element(new c_checkbox("hold firing animation",
					&vars.misc.hold_firinganims));

				main_child->add_element(new c_colorpicker(&vars.misc.watermark_color, color_t(255, 255, 255, 255)));
				main_child->add_element(new c_text("watermark line color"));

				main_child->add_element(new c_text("viewmodel offset"));

				main_child->add_element(new c_slider("", &vars.misc.viewmodel_x, -50.f, 50.f, "x: %.0f"));												  
																							  
				main_child->add_element(new c_slider("", &vars.misc.viewmodel_y, -50.f, 50.f, "y: %.0f"));												  
																							  
				main_child->add_element(new c_slider("", &vars.misc.viewmodel_z, -50.f, 50.f, "z: %.0f"));		

				main_child->add_element(new c_checkbox("buy bot",
					&vars.misc.autobuy.enable));

				main_child->add_element(new c_combo("primary weapon",
					&vars.misc.autobuy.main, {
						"auto sniper",
						"scout",
						"awp"
					}, []() { return vars.misc.autobuy.enable; }));

				main_child->add_element(new c_combo("secondary weapon",
					&vars.misc.autobuy.pistol, {
						"dual beretta",
						"heavy pistol"
					}, []() { return vars.misc.autobuy.enable; }));

				main_child->add_element(new c_multicombo("other",
					&vars.misc.autobuy.misc, {
						"head helmet",
						"other helmet",
						"he grenade",
						"molotov",
						"smoke",
						"taser",
						"defuse kit",
					}, []() { return vars.misc.autobuy.enable; }));

				main_child->initialize_elements();
			}
			window->add_element(main_child);

			reinit_config();
		}
		// undefined tab
		{
			auto undef_child = new c_child("", tab_t::undefined, window);

			undef_child->set_position(Vector2D(0, 0));
			undef_child->set_size(Vector2D(200, 200));

			std::stringstream s;
			s << "Hello user " << csgo->username;


			undef_child->add_element(new c_text(s.str(), nullptr, color_t(255, 0, 0))); // add steam name
			
			
			undef_child->initialize_elements();
			window->add_element(undef_child);
		}

		window->set_active_tab_index(tab_t::undefined);
		initialized = true;
	}

	if (!vars.menu.open) {
		if (window->get_transparency() > 0.f)
			window->decrease_transparency(animation_speed * 35.f);
	}
	
	ImGui::GetIO().MouseDrawCursor = window->get_transparency() > 0;
	POINT mp;
	GetCursorPos(&mp);
	ScreenToClient(GetForegroundWindow(), &mp);
	g_mouse.x = mp.x;
	g_mouse.y = mp.y;
	if (should_reinit_weapon_cfg) {
		reinit_weapon_cfg();
		should_reinit_weapon_cfg = false;
	}
	if (should_reinit_chams) {
		reinit_chams();
		should_reinit_chams = false;
	}
	if (should_reinit_config) {
		reinit_config();
		should_reinit_config = false;
	}
	
	window->render();
	if (window->g_hovered_element) {
		if (window->g_hovered_element->type == c_elementtype::input_text)
			ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
	}
	csgo->scroll_amount = 0;
}

void c_menu::update_binds()
{
	for (auto e : window->elements) {
		if (e->type == c_elementtype::child) {
			for (auto el : ((c_child*)e)->elements) {
				if (el->type == c_elementtype::checkbox) {
					auto c = (c_checkbox*)el;
					auto binder = c->bind;
					if (binder) {
						binder->key = std::clamp<unsigned int>(binder->key, 0, 255);

						if (binder->type == 2 && binder->key > 0) {
							if (window->key_updated(binder->key)) {
								*(bool*)c->get_ptr() = !(*(bool*)c->get_ptr());
							}
						}
						else if (binder->type == 1 && binder->key > 0) {
							*(bool*)c->get_ptr() = csgo->key_pressed[binder->key];
						}
						else if (binder->type == 3 && binder->key > 0) {
							*(bool*)c->get_ptr() = !csgo->key_pressed[binder->key];
						}
						binder->active = *(bool*)c->get_ptr();
					}
				}
				else if (el->type == c_elementtype::keybind) {
					auto c = (c_keybind*)el;
					auto binder = ((c_keybind*)el)->bind;
					if (binder) {
						binder->key = std::clamp<unsigned int>(binder->key, 0, 255);

						if (binder->type == 2 && binder->key > 0) {
							if (window->key_updated(binder->key)) {
								binder->active = !binder->active;
							}
						}
						else if (binder->type == 1 && binder->key > 0) {
							binder->active = csgo->key_pressed[binder->key];
						}
						else if (binder->type == 3 && binder->key > 0) {
							binder->active = !csgo->key_pressed[binder->key];
						}
						else if (binder->type == 0)
							binder->active = false;
						else if (binder->type == 4)
							binder->active = true;
					}
				}
			}
		}
	}
}

bool override_default() {
	return vars.ragebot.enable && (vars.ragebot.active_index == 0 || vars.ragebot.weapon[vars.ragebot.active_index].enable);
}

void c_menu::reinit_weapon_cfg()
{
	for (int i = 0; i < window->elements.size(); i++) {
		auto& e = window->elements[i];
		if (((c_child*)e)->get_title() == "aimbot settings") {
			window->elements.erase(window->elements.begin() + i);
			break;
		}
	}
	auto cfg_child = new c_child("aimbot settings", tab_t::rage, window);
	cfg_child->set_size(Vector2D(250, 520));
	cfg_child->set_position(Vector2D(258, 0)); {
		cfg_child->add_element(new c_combo("weapon", &vars.ragebot.active_index, {
			"default",
			"scar",
			"scout",
			"awp",
			"rifles",
			"pistols",
			"heavy pistols"
			}, enable_rage, [](int) { g_Menu->should_reinit_weapon_cfg = true; }));

		cfg_child->add_element(new c_checkbox("override default", &vars.ragebot.weapon[vars.ragebot.active_index].enable,
			[]() { return enable_rage() && vars.ragebot.active_index > 0; }));

		cfg_child->add_element(new c_slider("hitchance", &vars.ragebot.weapon[vars.ragebot.active_index].hitchance, 0, 100, "%.0f%%",
			override_default));

		cfg_child->add_element(new c_combo("hitchance type", &vars.ragebot.weapon[vars.ragebot.active_index].hitchancetype, {
			"trace ray",
			"inaccuracy",
		}, []() { return override_default() && vars.ragebot.weapon[vars.ragebot.active_index].hitchance > 0; }));

		cfg_child->add_element(new c_checkbox("quick stop", &vars.ragebot.weapon[vars.ragebot.active_index].quickstop,
			[]() { return override_default(); }));

		//cfg_child->add_element(new c_combo("quick stop", &vars.ragebot.weapon[vars.ragebot.active_index].quickstop, {
		//	"off",
		//	"full",
		//	"slide",
		//}, override_default));

		cfg_child->add_element(new c_multicombo("", &vars.ragebot.weapon[vars.ragebot.active_index].quickstop_options, {
			"between shots",
			"only when lethal",
		},
		[]() {
			return override_default() && vars.ragebot.weapon[vars.ragebot.active_index].quickstop;
		}));

		cfg_child->add_element(new c_slider("minimum damage", &vars.ragebot.weapon[vars.ragebot.active_index].mindamage, 0, 120, "%.0f hp",
			override_default));

		cfg_child->add_element(new c_slider("minimum damage [override]", &vars.ragebot.weapon[vars.ragebot.active_index].mindamage_override, 0, 120, "%.0f hp",
			[]() { return override_default() && vars.ragebot.override_dmg->key > 0; }));

		cfg_child->add_element(new c_multicombo("hitboxes", &vars.ragebot.weapon[vars.ragebot.active_index].hitscan, {
			"head",
			"neck",
			"upper chest",
			"chest",
			"stomach",
			"pelvis",
			"arms",
			"legs",
			"feet",
		}, override_default));

		cfg_child->add_element(new c_checkbox("enable multipoint", &vars.ragebot.weapon[vars.ragebot.active_index].multipoint, override_default));

		cfg_child->add_element(new c_slider("head scale", &vars.ragebot.weapon[vars.ragebot.active_index].pointscale_head, 
			0, 100, "%.0f%%", []() { return override_default() && vars.ragebot.weapon[vars.ragebot.active_index].multipoint; }));

		cfg_child->add_element(new c_slider("body scale", &vars.ragebot.weapon[vars.ragebot.active_index].pointscale_body,
			0, 100, "%.0f%%", []() { return override_default() && vars.ragebot.weapon[vars.ragebot.active_index].multipoint; }));

		cfg_child->add_element(new c_slider("body aim when hp lower than", &vars.ragebot.weapon[vars.ragebot.active_index].baim_under_hp,
			0, 100, "%.0f hp", []() { return override_default(); }));

		cfg_child->add_element(new c_multicombo("override hitbox if", &vars.ragebot.weapon[vars.ragebot.active_index].baim, {
			"in air",
			"unresolved",
			"lethal"
		},
		 override_default));
		
		cfg_child->add_element(new c_slider("max missed shot count", &vars.ragebot.weapon[vars.ragebot.active_index].max_misses,
			0, 5, "%.0f", []() {
			return override_default()
			&& (vars.ragebot.weapon[vars.ragebot.active_index].baim & 2);
		}));

		cfg_child->add_element(new c_multicombo("hitbox on override", &vars.ragebot.weapon[vars.ragebot.active_index].hitscan_baim, {
			"chest",
			"stomach",
			"pelvis",
			"legs",
			"feet",
		},
		[]() { return
			override_default()
			&& (vars.ragebot.weapon[vars.ragebot.active_index].baim > 0 || 
			vars.ragebot.weapon[vars.ragebot.active_index].baim_under_hp > 0 
			|| vars.ragebot.force_body->key > 0);
		}));

		cfg_child->add_element(new c_checkbox("adaptive body aim", &vars.ragebot.weapon[vars.ragebot.active_index].adaptive_baim, 
			[]() {
			return override_default();
		}));
		cfg_child->initialize_elements();
	}
	window->add_element(cfg_child);
}

void c_menu::reinit_config() {
	for (int i = 0; i < window->elements.size(); i++) {
		auto& e = window->elements[i];
		if (((c_child*)e)->get_title() == "configs") {
			window->elements.erase(window->elements.begin() + i);
			break;
		}
	}

	RefreshConfigs();
	auto config_child = new c_child("configs", tab_t::misc, window);
	config_child->set_size(Vector2D(250, 520));
	config_child->set_position(Vector2D(258, 0)); {
		config_child->add_element(new c_listbox("configs", &vars.menu.active_config_index, ConfigList, 150.f));
		config_child->add_element(new c_input_text("config name", &vars.menu.active_config_name, false));

		config_child->add_element(new c_button("load", []() {
			Config.Load(ConfigList[vars.menu.active_config_index]);
		}, []() { return ConfigList.size() > 0 && vars.menu.active_config_index >= 0; }));

		config_child->add_element(new c_button("save", []() {
			Config.Save(ConfigList[vars.menu.active_config_index]);
		}, []() { return ConfigList.size() > 0 && vars.menu.active_config_index >= 0; }));

		config_child->add_element(new c_button("refresh", []() { g_Menu->should_reinit_config = true; }));

		config_child->add_element(new c_button("create", []() {
			string add;
			if (vars.menu.active_config_name.find(".json") == -1)
				add = ".json";
			Config.Save(vars.menu.active_config_name + add);
			g_Menu->should_reinit_config = true;
			vars.menu.active_config_name.clear();
		}));

		config_child->add_element(new c_button("reset to default", []() { Config.ResetToDefault(); },
			[]() { return ConfigList.size() > 0 && vars.menu.active_config_index >= 0; }));

		config_child->initialize_elements();
	}
	window->add_element(config_child);
}

void c_menu::reinit_chams() {
	for (int i = 0; i < window->elements.size(); i++) {
		auto& e = window->elements[i];
		if (((c_child*)e)->get_title() == "models") {
			window->elements.erase(window->elements.begin() + i);
			break;
		}
	}
	auto cfg_child = new c_child("models", tab_t::esp, window);
	cfg_child->set_size(Vector2D(250, 520));
	cfg_child->set_position(Vector2D(0, 0)); {
		cfg_child->add_element(new c_combo("category", &vars.visuals.active_chams_index, {
				"enemy",
				"shadow",
				"local",
				"desync",
				"arms",
				"weapon",
				"glow"
			}, nullptr, [](int) { g_Menu->should_reinit_chams = true; }));

		switch (vars.visuals.active_chams_index)
		{
		case 0: { // enemy
			cfg_child->add_element(new c_colorpicker(&vars.visuals.chamscolor,
				color_t(255, 150, 0, 255),  []() { return vars.visuals.chams; }));
			cfg_child->add_element(new c_checkbox("on enemy", &vars.visuals.chams));

			cfg_child->add_element(new c_combo("overlay", &vars.visuals.overlay, {
					"off",
					"glow outline",
					"glow fade",
				}, []() { return vars.visuals.chams; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.glow_col,
				color_t(255, 255, 255, 0),  []() { return vars.visuals.chams && vars.visuals.overlay > 0; }));
			cfg_child->add_element(new c_text("overlay color",  []() { return vars.visuals.chams && vars.visuals.overlay > 0; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.chamscolor_xqz,
				color_t(0, 100, 255, 255),  []() { return vars.visuals.chamsxqz; }));
			cfg_child->add_element(new c_checkbox("through walls", &vars.visuals.chamsxqz));

			cfg_child->add_element(new c_combo("overlay", &vars.visuals.overlay_xqz, {
					"off",
					"glow outline",
					"glow fade",
				},  []() { return vars.visuals.chamsxqz; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.glow_col_xqz,
				color_t(255, 255, 255, 0),  []() { return vars.visuals.chamsxqz && vars.visuals.overlay_xqz > 0; }));
			cfg_child->add_element(new c_text("overlay color",  []() { return vars.visuals.chamsxqz && vars.visuals.overlay_xqz > 0; }));

			cfg_child->add_element(new c_combo("material", &vars.visuals.chamstype, {
					"normal",
					"flat",
					"metallic",
				},  []() { return vars.visuals.chams || vars.visuals.chamsxqz; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.metallic_clr,
				color_t(255, 255, 255, 255),  []()
			{ return vars.visuals.chamstype == 2 && (vars.visuals.chams || vars.visuals.chamsxqz); }));
			cfg_child->add_element(new c_text("metallic",  []()
			{ return vars.visuals.chamstype == 2 && (vars.visuals.chams || vars.visuals.chamsxqz); }));
			cfg_child->add_element(new c_colorpicker(&vars.visuals.metallic_clr2,
				color_t(255, 255, 255, 255),  []()
			{ return vars.visuals.chamstype == 2 && (vars.visuals.chams || vars.visuals.chamsxqz); }));
			cfg_child->add_element(new c_text("phong",  []()
			{ return vars.visuals.chamstype == 2 && (vars.visuals.chams || vars.visuals.chamsxqz); }));

			cfg_child->add_element(new c_slider("", &vars.visuals.phong_exponent, 0, 100, "exponent: %.0f",
				 []()
			{ return vars.visuals.chamstype == 2 && (vars.visuals.chams || vars.visuals.chamsxqz); }));
			cfg_child->add_element(new c_slider("", &vars.visuals.phong_boost, 0, 100, "boost: %.0f%%",
				 []()
			{ return vars.visuals.chamstype == 2 && (vars.visuals.chams || vars.visuals.chamsxqz); }));

			cfg_child->add_element(new c_slider("rim light", &vars.visuals.rim, -100.f, 100.f, "%.0f",
				 []()
			{ return vars.visuals.chamstype == 2 && (vars.visuals.chams || vars.visuals.chamsxqz); }));
			cfg_child->add_element(new c_slider("pearlescent", &vars.visuals.pearlescent, -100.f, 100.f, "%.0f%%",
				 []()
			{ return vars.visuals.chamstype == 2 && (vars.visuals.chams || vars.visuals.chamsxqz); }));

			cfg_child->add_element(new c_slider("brightness", &vars.visuals.chams_brightness, 0.f, 300.f, "%.0f%%",
				 []()
			{ return vars.visuals.chams || vars.visuals.chamsxqz; }));
		}
			break;
		case 1: // history
		{
			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[history].clr,
				color_t(255, 255, 255, 255),  []() { return vars.visuals.misc_chams[history].enable; }));
			cfg_child->add_element(new c_checkbox("on history", &vars.visuals.misc_chams[history].enable));
			cfg_child->add_element(new c_checkbox("interpolated", &vars.visuals.interpolated_bt));
			cfg_child->add_element(new c_combo("material", &vars.visuals.misc_chams[history].material, {
					"normal",
					"flat",
					"metallic",
				},  []() { return vars.visuals.misc_chams[history].enable; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[history].metallic_clr,
				color_t(255, 255, 255, 255),  []()
			{ return vars.visuals.misc_chams[history].material == 2 && vars.visuals.misc_chams[history].enable; }));

			cfg_child->add_element(new c_text("metallic",  []()
			{ return vars.visuals.misc_chams[history].material == 2 && vars.visuals.misc_chams[history].enable; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[history].metallic_clr2,
				color_t(255, 255, 255, 255),  []()
			{ return vars.visuals.misc_chams[history].material == 2 && vars.visuals.misc_chams[history].enable; }));

			cfg_child->add_element(new c_text("phong",  []()
			{ return vars.visuals.misc_chams[history].material == 2 && vars.visuals.misc_chams[history].enable; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.misc_chams[history].phong_exponent, 0, 100, "exponent: %.0f",
				 []()
			{ return vars.visuals.misc_chams[history].material == 2 && vars.visuals.misc_chams[history].enable; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.misc_chams[history].phong_boost, 0, 100, "boost: %.0f%%",
				 []()
			{ return vars.visuals.misc_chams[history].material == 2 && vars.visuals.misc_chams[history].enable; }));

			cfg_child->add_element(new c_slider("rim light", &vars.visuals.misc_chams[history].rim, -100, 100, "%.0f",
				 []()
			{ return vars.visuals.misc_chams[history].material == 2 && vars.visuals.misc_chams[history].enable; }));

			cfg_child->add_element(new c_slider("pearlescent", &vars.visuals.misc_chams[history].pearlescent, -100.f, 100.f, "%.0f",
				 []()
			{ return vars.visuals.misc_chams[history].material == 2 && vars.visuals.misc_chams[history].enable; }));

			{
				cfg_child->add_element(new c_combo("overlay", &vars.visuals.misc_chams[history].overlay, {
					"off",
					"glow outline",
					"glow fade",
					},  []() { return vars.visuals.misc_chams[history].enable; }));

				cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[history].glow_clr,
					color_t(255, 255, 255, 255),  []() { return vars.visuals.misc_chams[history].enable && vars.visuals.misc_chams[history].overlay > 0; }));
				cfg_child->add_element(new c_text("overlay color",  []() { return vars.visuals.misc_chams[history].enable && vars.visuals.misc_chams[history].overlay > 0; }));
			}

			cfg_child->add_element(new c_slider("brightness", &vars.visuals.misc_chams[history].chams_brightness, 0.f, 300.f, "%.0f%%",
				 []()
			{ return vars.visuals.misc_chams[history].enable; }));

		}
		break;
		case 2: // local
		{
			cfg_child->add_element(new c_colorpicker(&vars.visuals.localchams_color,
				color_t(255, 255, 255, 255),  []() { return vars.visuals.localchams; }));
			cfg_child->add_element(new c_checkbox("on local", &vars.visuals.localchams));
			cfg_child->add_element(new c_checkbox("interpolated", &vars.visuals.interpolated_model));
	
			cfg_child->add_element(new c_combo("material", &vars.visuals.localchamstype, {
					"normal",
					"flat",
					"metallic",
				},  []() { return vars.visuals.localchams; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.local_chams.metallic_clr,
				color_t(255, 255, 255, 255),  []()
				{ return vars.visuals.localchamstype == 2 && vars.visuals.localchams; }));

			cfg_child->add_element(new c_text("metallic",  []()
				{ return vars.visuals.localchamstype == 2 && vars.visuals.localchams; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.local_chams.metallic_clr2,
				color_t(255, 255, 255, 255),  []()
				{ return vars.visuals.localchamstype == 2 && vars.visuals.localchams; }));

			cfg_child->add_element(new c_text("phong",  []()
				{ return vars.visuals.localchamstype == 2 && vars.visuals.localchams; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.local_chams.phong_exponent, 0, 100, "exponent: %.0f",
				 []()
				{ return vars.visuals.localchamstype == 2 && vars.visuals.localchams; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.local_chams.phong_boost, 0, 100, "boost: %.0f%%",
				 []()
				{ return vars.visuals.localchamstype == 2 && vars.visuals.localchams; }));

			cfg_child->add_element(new c_slider("rim light", &vars.visuals.local_chams.rim, -100, 100, "%.0f",
				 []()
				{ return vars.visuals.localchamstype == 2 && vars.visuals.localchams; }));

			cfg_child->add_element(new c_slider("pearlescent", &vars.visuals.local_chams.pearlescent, -100.f, 100.f, "%.0f",
				 []()
				{ return vars.visuals.localchamstype == 2 && vars.visuals.localchams; }));

			{
				cfg_child->add_element(new c_combo("overlay", &vars.visuals.local_chams.overlay, {
					"off",
					"glow outline",
					"glow fade",
					},  []() { return vars.visuals.localchams; }));

				cfg_child->add_element(new c_colorpicker(&vars.visuals.local_glow_color,
					color_t(255, 255, 255, 255),  []() { return vars.visuals.localchams && vars.visuals.local_chams.overlay > 0; }));
				cfg_child->add_element(new c_text("overlay color",  []() { return vars.visuals.localchams && vars.visuals.local_chams.overlay > 0; }));
			}

			cfg_child->add_element(new c_slider("brightness", &vars.visuals.local_chams_brightness, 0.f, 300.f, "%.0f%%",
				 []()
				{ return vars.visuals.localchams; }));

			cfg_child->add_element(new c_checkbox("blend on scope", &vars.visuals.blend_on_scope));

			cfg_child->add_element(new c_slider("", &vars.visuals.blend_value, 0.f, 100.f, "%.0f",
				 []() { return vars.visuals.blend_on_scope; }));

		}
		break;
		case 3: // desync
		{
			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[desync].clr,
				color_t(255, 255, 255, 255),  []() { return vars.visuals.misc_chams[desync].enable; }));
			cfg_child->add_element(new c_checkbox("on desync", &vars.visuals.misc_chams[desync].enable));
			cfg_child->add_element(new c_checkbox("interpolated", &vars.visuals.interpolated_dsy));

			cfg_child->add_element(new c_combo("material", &vars.visuals.misc_chams[desync].material, {
					"normal",
					"flat",
					"metallic",
				},  []() { return vars.visuals.misc_chams[desync].enable; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[desync].metallic_clr,
				color_t(255, 255, 255, 255),  []()
				{ return vars.visuals.misc_chams[desync].material == 2 && vars.visuals.misc_chams[desync].enable; }));

			cfg_child->add_element(new c_text("metallic",  []()
				{ return vars.visuals.misc_chams[desync].material == 2 && vars.visuals.misc_chams[desync].enable; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[desync].metallic_clr2,
				color_t(255, 255, 255, 255),  []()
				{ return vars.visuals.misc_chams[desync].material == 2 && vars.visuals.misc_chams[desync].enable; }));

			cfg_child->add_element(new c_text("phong",  []()
				{ return vars.visuals.misc_chams[desync].material == 2 && vars.visuals.misc_chams[desync].enable; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.misc_chams[desync].phong_exponent, 0, 100, "exponent: %.0f",
				 []()
				{ return vars.visuals.misc_chams[desync].material == 2 && vars.visuals.misc_chams[desync].enable; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.misc_chams[desync].phong_boost, 0, 100, "boost: %.0f%%",
				 []()
				{ return vars.visuals.misc_chams[desync].material == 2 && vars.visuals.misc_chams[desync].enable; }));

			cfg_child->add_element(new c_slider("rim light", &vars.visuals.misc_chams[desync].rim, -100, 100, "%.0f",
				 []()
				{ return vars.visuals.misc_chams[desync].material == 2 && vars.visuals.misc_chams[desync].enable; }));

			cfg_child->add_element(new c_slider("pearlescent", &vars.visuals.misc_chams[desync].pearlescent, -100.f, 100.f, "%.0f",
				 []()
				{ return vars.visuals.misc_chams[desync].material == 2 && vars.visuals.misc_chams[desync].enable; }));

			{
				cfg_child->add_element(new c_combo("overlay", &vars.visuals.misc_chams[desync].overlay, {
					"off",
					"glow outline",
					"glow fade",
					},  []() { return vars.visuals.misc_chams[desync].enable; }));

				cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[desync].glow_clr,
					color_t(255, 255, 255, 255),  []() { return vars.visuals.misc_chams[desync].enable && vars.visuals.misc_chams[desync].overlay > 0; }));
				cfg_child->add_element(new c_text("overlay color",  []() { return vars.visuals.misc_chams[desync].enable && vars.visuals.misc_chams[desync].overlay > 0; }));
			}

			cfg_child->add_element(new c_slider("brightness", &vars.visuals.misc_chams[desync].chams_brightness, 0.f, 300.f, "%.0f%%",
				 []()
				{ return vars.visuals.misc_chams[desync].enable; }));

		}
		break;
		case 4: // arms
		{
			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[arms].clr,
				color_t(255, 255, 255, 255),  []() { return vars.visuals.misc_chams[arms].enable; }));
			cfg_child->add_element(new c_checkbox("on arms", &vars.visuals.misc_chams[arms].enable));

			cfg_child->add_element(new c_combo("material", &vars.visuals.misc_chams[arms].material, {
					"normal",
					"flat",
					"metallic",
				},  []() { return vars.visuals.misc_chams[arms].enable; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[arms].metallic_clr,
				color_t(255, 255, 255, 255),  []()
				{ return vars.visuals.misc_chams[arms].material == 2 && vars.visuals.misc_chams[arms].enable; }));

			cfg_child->add_element(new c_text("metallic",  []()
				{ return vars.visuals.misc_chams[arms].material == 2 && vars.visuals.misc_chams[arms].enable; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[arms].metallic_clr2,
				color_t(255, 255, 255, 255),  []()
				{ return vars.visuals.misc_chams[arms].material == 2 && vars.visuals.misc_chams[arms].enable; }));

			cfg_child->add_element(new c_text("phong",  []()
				{ return vars.visuals.misc_chams[arms].material == 2 && vars.visuals.misc_chams[arms].enable; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.misc_chams[arms].phong_exponent, 0.f, 100.f, "exponent: %.0f",
				 []()
				{ return vars.visuals.misc_chams[arms].material == 2 && vars.visuals.misc_chams[arms].enable; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.misc_chams[arms].phong_boost, 0.f, 10.f, "boost: %.0f%%",
				 []()
				{ return vars.visuals.misc_chams[arms].material == 2 && vars.visuals.misc_chams[arms].enable; }));

			cfg_child->add_element(new c_slider("rim light", &vars.visuals.misc_chams[arms].rim, -100.f, 100.f, "%.0f",
				 []()
				{ return vars.visuals.misc_chams[arms].material == 2 && vars.visuals.misc_chams[arms].enable; }));

			cfg_child->add_element(new c_slider("pearlescent", &vars.visuals.misc_chams[arms].pearlescent, -100.f, 100.f, "%.0f",
				 []()
				{ return vars.visuals.misc_chams[arms].material == 2 && vars.visuals.misc_chams[arms].enable; }));

			{
				cfg_child->add_element(new c_combo("overlay", &vars.visuals.misc_chams[arms].overlay, {
					"off",
					"glow outline",
					"glow fade",
					},  []() { return vars.visuals.misc_chams[arms].enable; }));

				cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[arms].glow_clr,
					color_t(255, 255, 255, 255),  []() { return vars.visuals.misc_chams[arms].enable && vars.visuals.misc_chams[arms].overlay > 0; }));
				cfg_child->add_element(new c_text("overlay color",  []() { return vars.visuals.misc_chams[arms].enable && vars.visuals.misc_chams[arms].overlay > 0; }));
			}

			cfg_child->add_element(new c_slider("brightness", &vars.visuals.misc_chams[arms].chams_brightness, 0.f, 300.f, "%.0f%%",
				 []()
				{ return vars.visuals.misc_chams[arms].enable; }));

		}
		break;
		case 5: // weapon
		{
			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[weapon].clr,
				color_t(255, 255, 255, 255),  []() { return vars.visuals.misc_chams[weapon].enable; }));
			cfg_child->add_element(new c_checkbox("on weapon", &vars.visuals.misc_chams[weapon].enable));

			cfg_child->add_element(new c_combo("material", &vars.visuals.misc_chams[weapon].material, {
					"normal",
					"flat",
					"metallic",
				},  []() { return vars.visuals.misc_chams[weapon].enable; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[weapon].metallic_clr,
				color_t(255, 255, 255, 255),  []()
				{ return vars.visuals.misc_chams[weapon].material == 2 && vars.visuals.misc_chams[weapon].enable; }));

			cfg_child->add_element(new c_text("metallic",  []()
				{ return vars.visuals.misc_chams[weapon].material == 2 && vars.visuals.misc_chams[weapon].enable; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[weapon].metallic_clr2,
				color_t(255, 255, 255, 255),  []()
				{ return vars.visuals.misc_chams[weapon].material == 2 && vars.visuals.misc_chams[weapon].enable; }));

			cfg_child->add_element(new c_text("phong",  []()
				{ return vars.visuals.misc_chams[weapon].material == 2 && vars.visuals.misc_chams[weapon].enable; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.misc_chams[weapon].phong_exponent, 0.f, 100.f, "exponent: %.0f",
				 []()
				{ return vars.visuals.misc_chams[weapon].material == 2 && vars.visuals.misc_chams[weapon].enable; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.misc_chams[weapon].phong_boost, 0.f, 100.f, "boost: %.0f%%",
				 []()
				{ return vars.visuals.misc_chams[weapon].material == 2 && vars.visuals.misc_chams[weapon].enable; }));

			cfg_child->add_element(new c_slider("rim light", &vars.visuals.misc_chams[weapon].rim, -100.f, 100.f, "%.0f",
				 []()
				{ return vars.visuals.misc_chams[weapon].material == 2 && vars.visuals.misc_chams[weapon].enable; }));

			cfg_child->add_element(new c_slider("pearlescent", &vars.visuals.misc_chams[weapon].pearlescent, -100.f, 100.f, "%.0f",
				 []()
				{ return vars.visuals.misc_chams[weapon].material == 2 && vars.visuals.misc_chams[weapon].enable; }));

			{
				cfg_child->add_element(new c_combo("overlay", &vars.visuals.misc_chams[weapon].overlay, {
					"off",
					"glow outline",
					"glow fade",
					},  []() { return vars.visuals.misc_chams[weapon].enable; }));

				cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[weapon].glow_clr,
					color_t(255, 255, 255, 255),  []() { return vars.visuals.misc_chams[weapon].enable && vars.visuals.misc_chams[weapon].overlay > 0; }));
				cfg_child->add_element(new c_text("overlay color",  []() { return vars.visuals.misc_chams[weapon].enable && vars.visuals.misc_chams[weapon].overlay > 0; }));
			}

			cfg_child->add_element(new c_slider("brightness", &vars.visuals.misc_chams[weapon].chams_brightness, 0.f, 300.f, "%.0f%%",
				 []()
				{ return vars.visuals.misc_chams[weapon].enable; }));

		}
		break;
		case 6: // glow
		{
			cfg_child->add_element(new c_colorpicker(&vars.visuals.glow_color,
				color_t(255, 0, 255, 150),  []()
				{ return vars.visuals.glow; }));

			cfg_child->add_element(new c_checkbox("on entity", &vars.visuals.glow));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.local_glow_clr,
				color_t(10, 255, 100, 150),  []()
				{ return vars.visuals.local_glow; }));

			cfg_child->add_element(new c_checkbox("on local", &vars.visuals.local_glow));

			cfg_child->add_element(new c_combo("glow style", &vars.visuals.glowtype, {
				"normal",
				"pulsating",
			},  []() { return vars.visuals.glow || vars.visuals.local_glow; }));
		}
		break;

		}
	}
	cfg_child->initialize_elements();
	window->add_element(cfg_child);
}

c_menu* g_Menu = new c_menu();