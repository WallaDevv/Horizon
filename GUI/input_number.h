#pragma once
#include "element.h"

class c_input_number : public c_element {
private:
	std::string label;
	float animation, pulsating;
	bool active, b_switch;
	bool(*should_render)();
	bool password;
	void* value;
public:
	c_input_number(string label, void *value, bool password, bool(*should_render)() = nullptr) {
		this->label = label;
		this->value = value;
		this->password = password;
		b_switch = false;
		active = false;
		this->type = input_number;
		this->animation = 0.f;
		this->should_render = should_render;
	}
	int get_format() {
		return *(int*)value;
	}
	void change_pointer(void* ptr) { };
	bool update();
	bool hovered();
	void render();
	void special_render() {};
	int get_total_offset() {
		if (should_render)
			if (!should_render())
				return 0;
		return 23 + (label.size() > 0) * 15;
	};
};