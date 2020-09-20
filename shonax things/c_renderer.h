#pragma once


#include <unordered_map>
#include "../Vector.h"
#include "../colors.h"
#include <functional>
#include "security/fnv1a.h"
#include <d3d9.h>
#include "d3dfont.h"

class c_renderer
{
	struct vertex {
		float x, y, z, rhw;
		uint32_t color;
	};

public:
	static void create_renderer(IDirect3DDevice9* _dev);

	static void line(Vector2D from, Vector2D to, color_t color);

	static void rect(Vector2D from, Vector2D size, color_t color);
	void rect_linear_gradient(Vector2D from, Vector2D size, color_t color1, color_t color2, bool horizontal) const;
	static void rect_full_linear_gradient(Vector2D from, Vector2D size, color_t color1, color_t color2, color_t color3, color_t color4);
	static void rect_filled(Vector2D from, Vector2D size, color_t color);
	void rect_filled_linear_gradient(Vector2D from, Vector2D size, color_t color1, color_t color2, bool horizontal = false) const;
	void rect_filled_radial_gradient(Vector2D from, Vector2D size, color_t color1, color_t color2);
	void rect_filled_diamond_gradient(Vector2D from, Vector2D size, color_t color1, color_t color2) const;

	void parallelogram(Vector2D from, Vector2D size, color_t color, uint8_t side = 0, float radius = 8.f) const;
	void parallelogram_filled_linear_gradient(Vector2D from, Vector2D size, color_t color1, color_t color2,
		uint8_t side = 0, bool horizontal = false, float radius = 8.f) const;

	static void triangle(Vector2D pos1, Vector2D pos2, Vector2D pos3, color_t color);
	void triangle_linear_gradient(Vector2D pos1, Vector2D pos2, Vector2D pos3, color_t color1,
		color_t color2, color_t color3) const;
	static void triangle_filled(Vector2D pos1, Vector2D pos2, Vector2D pos3, color_t color);
	static void triangle_filled_linear_gradient(Vector2D pos1, Vector2D pos2, Vector2D pos3, color_t color1,
		color_t color2, color_t color3);

	void circle(Vector2D center, float radius, color_t color);
	static void circle_filled(Vector2D center, float radius, color_t color);
	void circle_filled_radial_gradient(Vector2D center, float radius, color_t color1, color_t color2);

	static void text(Vector2D pos, const char* str, color_t color, uint32_t font = default_font, uint8_t flags = 0);
	static Vector2D get_text_size(const char* str, uint32_t font);

	void create_texture(void* src, uint32_t size, LPDIRECT3DTEXTURE9* texture) const;
	void create_sprite(LPD3DXSPRITE* sprite) const;
	static void image(Vector2D position, LPDIRECT3DTEXTURE9 texture, LPD3DXSPRITE sprite, float scl = 1.f, float alpha = 1.f);

	static void scale(Vector2D& vec);
	static Vector2D get_center();

	static viewmatrix& world_to_screen_matrix();
	static bool screen_transform(const Vector& in, Vector2D& out, viewmatrix& matrix);
	static bool screen_transform_fix(const Vector& in, Vector2D& out, viewmatrix& matrix);
	inline bool is_on_screen(const Vector& in, float width, viewmatrix& matrix) const;

	void refresh_viewport();
	static void init_device_objects(IDirect3DDevice9* dev);
	static void invalidate_device_objects();
	void setup_render_state() const;
	void register_reset_handler(std::function<void()> handler);

private:
	static constexpr auto points = 64;
	static constexpr auto points_sphere_latitude = 16;
	static constexpr auto points_sphere_longitude = 24;
	static constexpr auto default_font = fnv1a("pro13");
	inline static IDirect3DDevice9* dev;
	inline static D3DVIEWPORT9 port;
	inline static std::vector<Vector2D> lookup_table;
	inline static std::vector<Vector> lookup_sphere;
	inline static std::vector<std::function<void()>> reset_handlers;
	static void build_lookup_table();
	inline static std::unordered_map<uint32_t, c_font> fonts;
};
