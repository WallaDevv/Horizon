#pragma once
#include "../hooks/Hooks.h"
#include "../shonax things/c_renderer.h"
static int showtime = 5.f;
static string prefix = "[Horizon] - ";


class CMessage
{
public:
	CMessage(string str, float time, color_t clr)
	{
		this->str = str;
		this->time = csgo->get_absolute_time();
		this->clr = clr;
		alpha = 255;
	}
	std::string str;
	float time;
	color_t clr;
	int alpha;
};

class CEventlog
{
public:
	std::vector <CMessage> messages;
	void Draw()
	{
		if (!interfaces.engine->IsConnected() || !interfaces.engine->IsInGame()) {
			messages.clear();
			return;
		}

		if (!vars.visuals.eventlog)
			return;
		while (messages.size() > 10)
			messages.erase(messages.begin());
		for (int i = messages.size() - 1; i >= 0; i--)
		{
			if (messages[i].time + showtime <= csgo->get_absolute_time())
				messages[i].alpha -= 1;

			c_renderer::text(Vector2D(15 - (255 - messages[i].alpha), 50 + (15 * i)), messages[i].str.c_str(), color_t(messages[i].clr.get_red(), messages[i].clr.get_green(), messages[i].clr.get_blue(), messages[i].alpha), fnv1a("prime12"));

			if (messages[i].alpha <= 0)
				messages.erase(messages.begin() + i);
		}

	}
};