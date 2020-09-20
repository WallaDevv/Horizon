#include "c_renderer.h"
#include <utility>

#include "../fonts/prime.h"
#include "security/string_obfuscation.h"
#include "../hooks/Hooks.h"




void c_renderer::scale(Vector2D& vec)
{
	vec.x = (vec.x + 1.f) * port.Width / 2.f;
	vec.y = (-vec.y + 1.f) * port.Height / 2.f;
}

bool c_renderer::screen_transform(const Vector& in, Vector2D& out, viewmatrix& matrix)
{
	if (screen_transform_fix(in, out, matrix))
	{
		int w, h;
		interfaces.engine->GetScreenSize(w, h);

		out.x = (w / 2.0f) + (out.x * w) / 2.0f;
		out.y = (h / 2.0f) - (out.y * h) / 2.0f;

		return true;
	}
	return false;
}

bool c_renderer::screen_transform_fix(const Vector& in, Vector2D& out, viewmatrix& matrix)
{
	out.x = matrix[0][0] * in.x + matrix[0][1] * in.y + matrix[0][2] * in.z + matrix[0][3];
	out.y = matrix[1][0] * in.x + matrix[1][1] * in.y + matrix[1][2] * in.z + matrix[1][3];

	const auto w = matrix[3][0] * in.x + matrix[3][1] * in.y + matrix[3][2] * in.z + matrix[3][3];

	if (w < 0.001f)
	{
		out.x *= 100000;
		out.y *= 100000;
		return false;
	}

	const auto invw = 1.0f / w;
	out.x *= invw;
	out.y *= invw;

	return true;
}








bool c_renderer::is_on_screen(const Vector& in, const float width, viewmatrix& matrix) const
{
	auto out = Vector2D(matrix[0][0] * in.x + matrix[0][1] * in.y + matrix[0][2] * in.z + matrix[0][3],
		matrix[1][0] * in.x + matrix[1][1] * in.y + matrix[1][2] * in.z + matrix[1][3]);

	const auto w = matrix[3][0] * in.x + matrix[3][1] * in.y + matrix[3][2] * in.z + matrix[3][3];

	if (w < 0.001f)
	{
		out.x *= 100000;
		out.y *= 100000;
		return false;
	}

	const auto invw = 1.0f / w;
	out.x *= invw;
	out.y *= invw;

	scale(out);
	return !(out.x - width > port.Width || out.x + width < 1.f);
}


void c_renderer::create_renderer(IDirect3DDevice9* _dev)
{
	DWORD fnt;
	dev = _dev;
	AddFontMemResourceEx(reinterpret_cast<void*>(prime_f), sizeof(prime_f), nullptr, &fnt);

	fonts[fnv1a("prime12")] = c_font("Prime", 7, FW_REGULAR);
	fonts[fnv1a("prime10")] = c_font("Prime", 8, FW_SEMIBOLD);
	init_device_objects(dev);
}

void c_renderer::line(const Vector2D from, const Vector2D to, const color_t color) 
{
	const auto col = color.direct();

	vertex vert[2] =
	{
		{ from.x, from.y, 0.0f, 1.0f, col },
		{ to.x, to.y, 0.0f, 1.0f, col }
	};

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);




	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	dev->SetRenderState(D3DRS_FOGENABLE, false);
	DWORD oldFVF;
	dev->GetFVF(&oldFVF);
	dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	dev->SetTexture(0, 0);
	dev->DrawPrimitiveUP(D3DPT_LINELIST, 1, &vert, sizeof(vertex));
	dev->SetFVF(oldFVF);
	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}


void c_renderer::rect(const Vector2D from, const Vector2D size, const color_t color) 
{
	const auto to = from + size;
	const auto col = color.direct();

	vertex vert[5] =
	{
		{ from.x, from.y, 0.0f, 1.0f, col },
		{ to.x, from.y, 0.0f, 1.0f, col },
		{ to.x, to.y, 0.0f, 1.0f, col },
		{ from.x, to.y, 0.0f, 1.0f, col },
		{ from.x, from.y, 0.0f, 1.0f, col }
	};
	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	dev->SetRenderState(D3DRS_FOGENABLE, false);
	DWORD oldFVF;
	dev->GetFVF(&oldFVF);
	dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	dev->SetTexture(0, 0);
	dev->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, &vert, sizeof(vertex));
	dev->SetFVF(oldFVF);
}

void c_renderer::rect_linear_gradient(const Vector2D from, const Vector2D size, const color_t color1, const color_t color2, const bool horizontal) const
{
	const auto to = from + size;
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();

	vertex vert[5] =
	{
		{ from.x, from.y, 0.0f, 1.0f, col2 },
	{ to.x, from.y, 0.0f, 1.0f, horizontal ? col2 : col1 },
	{ to.x, to.y, 0.0f, 1.0f, col2 },
	{ from.x, to.y, 0.0f, 1.0f, horizontal ? col1 : col2 },
	{ from.x, from.y, 0.0f, 1.0f, col1 }
	};

	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	dev->SetRenderState(D3DRS_FOGENABLE, false);
	DWORD oldFVF;
	dev->GetFVF(&oldFVF);
	dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	dev->SetTexture(0, 0);
	dev->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, &vert, sizeof(vertex));
	dev->SetFVF(oldFVF);
}

void c_renderer::rect_full_linear_gradient(const Vector2D from, const Vector2D size, color_t color1, color_t color2, color_t color3,
	color_t color4) 
{
	const auto to = from + size;
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();
	const auto col3 = color3.direct();
	const auto col4 = color4.direct();

	vertex vert[5] =
	{
		{ from.x, from.y, 0.0f, 1.0f, col1 },
	{ to.x, from.y, 0.0f, 1.0f, col2 },
	{ to.x, to.y, 0.0f, 1.0f, col3 },
	{ from.x, to.y, 0.0f, 1.0f, col4 },
	{ from.x, from.y, 0.0f, 1.0f, col1 }
	};

	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	dev->SetRenderState(D3DRS_FOGENABLE, false);
	DWORD oldFVF;
	dev->GetFVF(&oldFVF);
	dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	dev->SetTexture(0, 0);
	dev->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, &vert, sizeof(vertex));
	dev->SetFVF(oldFVF);
}

void c_renderer::rect_filled(const Vector2D from, const Vector2D size, const color_t color)
{
	const auto to = from + size;
	const auto col = color.direct();

	vertex vert[4] =
	{
		{ from.x, from.y, 0.0f, 1.0f, col },
	{ to.x, from.y, 0.0f, 1.0f, col },
	{ from.x, to.y, 0.0f, 1.0f, col },
	{ to.x, to.y, 0.0f, 1.0f, col }
	};

	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	dev->SetRenderState(D3DRS_FOGENABLE, false);
	DWORD oldFVF;
	dev->GetFVF(&oldFVF);
	dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	dev->SetTexture(0, 0);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vert, sizeof(vertex));
	dev->SetFVF(oldFVF);
}

void c_renderer::rect_filled_linear_gradient(const Vector2D from, const Vector2D size, const color_t color1,
	const color_t color2, const bool horizontal) const
{
	const auto to = from + size;
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();

	vertex vert[4] =
	{
		{ from.x, from.y, 0.0f, 1.0f, col1 },
	{ to.x, from.y, 0.0f, 1.0f, horizontal ? col2 : col1 },
	{ from.x, to.y, 0.0f, 1.0f, horizontal ? col1 : col2 },
	{ to.x, to.y, 0.0f, 1.0f, col2 }
	};

	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	dev->SetRenderState(D3DRS_FOGENABLE, false);
	DWORD oldFVF;
	dev->GetFVF(&oldFVF);
	dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	dev->SetTexture(0, 0);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vert, sizeof(vertex));
	dev->SetFVF(oldFVF);
}

void c_renderer::rect_filled_radial_gradient(const Vector2D from, const Vector2D size, const color_t color1,
	const color_t color2)
{
	const auto center = from + size / 2.0f;
	const auto radius = (center - from).Length();

	D3DVIEWPORT9 new_port;
	new_port.X = static_cast<uint32_t>(from.x);
	new_port.Y = static_cast<uint32_t>(from.y);
	new_port.Width = static_cast<uint32_t>(size.x);
	new_port.Height = static_cast<uint32_t>(size.y);

	dev->SetViewport(&new_port);
	circle_filled_radial_gradient(center, radius, color1, color2);
	dev->SetViewport(&port);
}

void c_renderer::rect_filled_diamond_gradient(const Vector2D from, const Vector2D size, const color_t color1,
	const color_t color2) const
{
	const auto to = from + size;
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();

	vertex vert[6] =
	{
		{ (from.x + to.x) / 2.0f, (from.y + to.y) / 2.0f, 0.0f, 1.0f, col2 },
	{ from.x, from.y, 0.0f, 1.0f, col1 },
	{ to.x, from.y, 0.0f, 1.0f, col1 },
	{ to.x, to.y, 0.0f, 1.0f, col1 },
	{ from.x, to.y, 0.0f, 1.0f, col1 },
	{ from.x, from.y, 0.0f, 1.0f, col1 }
	};

	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	dev->SetRenderState(D3DRS_FOGENABLE, false);
	DWORD oldFVF;
	dev->GetFVF(&oldFVF);
	dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	dev->SetTexture(0, 0);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 5, &vert, sizeof(vertex));
	dev->SetFVF(oldFVF);
}

void c_renderer::parallelogram(const Vector2D from, const Vector2D size, color_t color, const uint8_t side, const float radius) const
{
	const auto to = from + size;
	const auto col = color.direct();

	vertex vert[5] =
	{
		{ from.x + (side != 1 ? radius : 0.0f), from.y, 0.0f, 1.0f, col },
	{ to.x, from.y, 0.0f, 1.0f, col },
	{ to.x - (side != 2 ? radius : 0.0f), to.y, 0.0f, 1.0f, col },
	{ from.x, to.y, 0.0f, 1.0f, col },
	{ from.x + (side != 1 ? radius : 0.0f), from.y, 0.0f, 1.0f, col }
	};

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	dev->SetRenderState(D3DRS_FOGENABLE, false);
	DWORD oldFVF;
	dev->GetFVF(&oldFVF);
	dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	dev->SetTexture(0, 0);
	dev->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, &vert, sizeof(vertex));
	dev->SetFVF(oldFVF);
	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::parallelogram_filled_linear_gradient(const Vector2D from, const Vector2D size, color_t color1,
	color_t color2, const uint8_t side, const bool horizontal, const float radius) const
{
	const auto to = from + size;
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();

	vertex vert[4] =
	{
		{ from.x + (side != 1 ? radius : 0.0f), from.y, 0.0f, 1.0f, col1 },
	{ to.x, from.y, 0.0f, 1.0f, horizontal ? col2 : col1 },
	{ from.x, to.y, 0.0f, 1.0f, horizontal ? col1 : col2 },
	{ to.x - (side != 2 ? radius : 0.0f), to.y, 0.0f, 1.0f, col2 }
	};

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	dev->SetRenderState(D3DRS_FOGENABLE, false);
	DWORD oldFVF;
	dev->GetFVF(&oldFVF);
	dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	dev->SetTexture(0, 0);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vert, sizeof(vertex));
	dev->SetFVF(oldFVF);
	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::triangle(const Vector2D pos1, const Vector2D pos2, const Vector2D pos3, const color_t color)
{
	const auto col = color.direct();

	vertex vert[4] =
	{
		{ pos1.x, pos1.y, 0.0f, 1.0f, col },
	{ pos2.x, pos2.y, 0.0f, 1.0f, col },
	{ pos3.x, pos3.y, 0.0f, 1.0f, col },
	{ pos1.x, pos1.y, 0.0f, 1.0f, col }
	};

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	dev->SetRenderState(D3DRS_FOGENABLE, false);
	DWORD oldFVF;
	dev->GetFVF(&oldFVF);
	dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	dev->SetTexture(0, 0);
	dev->DrawPrimitiveUP(D3DPT_LINESTRIP, 3, &vert, sizeof(vertex));
	dev->SetFVF(oldFVF);
	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::triangle_linear_gradient(const Vector2D pos1, const Vector2D pos2, const Vector2D pos3, color_t color1, color_t color2, color_t color3) const
{
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();
	const auto col3 = color3.direct();

	vertex vert[4] =
	{
		{ pos1.x, pos1.y, 0.0f, 1.0f, col1 },
	{ pos2.x, pos2.y, 0.0f, 1.0f, col2 },
	{ pos3.x, pos3.y, 0.0f, 1.0f, col3 },
	{ pos1.x, pos1.y, 0.0f, 1.0f, col1 }
	};

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	dev->SetRenderState(D3DRS_FOGENABLE, false);
	DWORD oldFVF;
	dev->GetFVF(&oldFVF);
	dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	dev->SetTexture(0, 0);
	dev->DrawPrimitiveUP(D3DPT_LINESTRIP, 3, &vert, sizeof(vertex));
	dev->SetFVF(oldFVF);
	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::triangle_filled(const Vector2D pos1, const Vector2D pos2, const Vector2D pos3, const color_t color) 
{
	const auto col = color.direct();

	vertex vert[4] =
	{
		{ pos1.x, pos1.y, 0.0f, 1.0f, col },
	{ pos2.x, pos2.y, 0.0f, 1.0f, col },
	{ pos3.x, pos3.y, 0.0f, 1.0f, col }
	};

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	dev->SetRenderState(D3DRS_FOGENABLE, false);
	DWORD oldFVF;
	dev->GetFVF(&oldFVF);
	dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	dev->SetTexture(0, 0);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 1, &vert, sizeof(vertex));
	dev->SetFVF(oldFVF);
	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::triangle_filled_linear_gradient(const Vector2D pos1, const Vector2D pos2, const Vector2D pos3,
	const color_t color1, const color_t color2, const color_t color3)
{
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();
	const auto col3 = color3.direct();

	vertex vert[4] =
	{
		{ pos1.x, pos1.y, 0.0f, 1.0f, col1 },
	{ pos2.x, pos2.y, 0.0f, 1.0f, col2 },
	{ pos3.x, pos3.y, 0.0f, 1.0f, col3 }
	};

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	dev->SetRenderState(D3DRS_FOGENABLE, false);
	DWORD oldFVF;
	dev->GetFVF(&oldFVF);
	dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	dev->SetTexture(0, 0);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 1, &vert, sizeof(vertex));
	dev->SetFVF(oldFVF);
	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::circle(const Vector2D center, const float radius, const color_t color)
{
	const auto col = color.direct();
	build_lookup_table();

	vertex vert[points + 1] = {};

	for (auto i = 0; i <= points; i++)
		vert[i] =
	{
		center.x + radius * lookup_table[i].x,
		center.y - radius * lookup_table[i].y,
		0.0f,
		1.0f,
		col
	};

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	dev->SetRenderState(D3DRS_FOGENABLE, false);
	DWORD oldFVF;
	dev->GetFVF(&oldFVF);
	dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	dev->SetTexture(0, 0);
	dev->DrawPrimitiveUP(D3DPT_LINESTRIP, points, &vert, sizeof(vertex));
	dev->SetFVF(oldFVF);
	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::circle_filled(const Vector2D center, const float radius, const color_t color)
{
	const auto col = color.direct();
	build_lookup_table();

	vertex vert[points + 1] = {};

	for (auto i = 0; i <= points; i++)
		vert[i] =
	{
		center.x + radius * lookup_table[i].x,
		center.y - radius * lookup_table[i].y,
		0.0f,
		1.0f,
		col
	};

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);


	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	dev->SetRenderState(D3DRS_FOGENABLE, false);
	DWORD oldFVF;
	dev->GetFVF(&oldFVF);
	dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	dev->SetTexture(0, 0);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, points, &vert, sizeof(vertex));
	dev->SetFVF(oldFVF);
	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::circle_filled_radial_gradient(const Vector2D center, const float radius, const color_t color1,
	const color_t color2)
{
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();
	build_lookup_table();

	vertex vert[points + 2] = {};

	for (auto i = 1; i <= points; i++)
		vert[i] =
	{
		center.x + radius * lookup_table[i].x,
		center.y - radius * lookup_table[i].y,
		0.0f,
		1.0f,
		col1
	};

	vert[0] = { center.x, center.y, 0.0f, 1.0f, col2 };
	vert[points + 1] = vert[1];

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	dev->SetRenderState(D3DRS_FOGENABLE, false);
	DWORD oldFVF;
	dev->GetFVF(&oldFVF);
	dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	dev->SetTexture(0, 0);
	dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, points, &vert, sizeof(vertex));
	dev->SetFVF(oldFVF);
	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

void c_renderer::text(const Vector2D pos, const char* str, color_t color, const uint32_t font, const uint8_t flags)
{
	fonts.at(font).draw_string(std::roundf(pos.x), std::roundf(pos.y), color.direct(), str, flags);
}



Vector2D c_renderer::get_text_size(const char* str, const uint32_t font)
{
	SIZE size;
	fonts.at(font).get_text_extent(str, &size);
	return Vector2D(static_cast<float>(size.cx), static_cast<float>(size.cy));
}


void c_renderer::create_texture(void* src, const uint32_t size, LPDIRECT3DTEXTURE9* texture) const
{
	D3DXCreateTextureFromFileInMemory(dev, src, size, texture);
}

void c_renderer::create_sprite(LPD3DXSPRITE* sprite) const
{
	D3DXCreateSprite(dev, sprite);
}

void c_renderer::image(const Vector2D position, LPDIRECT3DTEXTURE9 texture, LPD3DXSPRITE sprite, const float scl, const float alpha)
{
	D3DXMATRIX world;
	D3DXMATRIX rotation;
	D3DXMATRIX scale;
	D3DXMATRIX translation;
	D3DXMatrixIdentity(&world);

	D3DXMatrixScaling(&scale, scl, scl, 1.f);
	D3DXMatrixRotationYawPitchRoll(&rotation, 0.f, 0.f, 0.f);
	D3DXMatrixTranslation(&translation, 0.f, 0.f, 0.f);
	world = rotation * scale * translation;

	D3DSURFACE_DESC img_info;
	texture->GetLevelDesc(0, &img_info);

	auto vec = D3DXVECTOR3(position.x, position.y, 0.f);
	sprite->SetTransform(&world);
	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	sprite->Draw(texture, nullptr, nullptr, &vec, D3DCOLOR_RGBA(255, 255, 255, static_cast<int>(255 * alpha)));
	sprite->End();
}

Vector2D c_renderer::get_center()
{
	return Vector2D(static_cast<float>(port.Width), static_cast<float>(port.Height)) / 2.f;
}
class fixrender {
public:
	
};
viewmatrix& c_renderer::world_to_screen_matrix()
{
	static auto view_matrix = *reinterpret_cast<uintptr_t*>(csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::client_dll::s().c_str())].c_str()), g_Patterns[fnv::hash(hs::world_to_screen_matrix::s().c_str())].c_str()) + 3) + 176;
	return *reinterpret_cast<viewmatrix*>(view_matrix);

	//static DWORD_PTR dwFunction = *reinterpret_cast<DWORD_PTR*>(csgo->Utils.FindPatternIDA(GetModuleHandleA("engine.dll"), ("A1 ? ? ? ? 83 F8 01 7E 11 69 C8")));
	//static DWORD_PTR dwVMatrixPtr = *(DWORD_PTR*)(dwFunction + 0x11);
	//
	//DWORD_PTR dwVMatrix = *(DWORD_PTR*)dwVMatrixPtr;
	//
	//DWORD_PTR dwResult = dwVMatrix + 2 * 528 - 68;
	//
	//return (viewmatrix&)(*(DWORD_PTR*)dwResult);
	//static fixrender* ff = *reinterpret_cast<fixrender**>(csgo->Utils.FindPatternIDA(GetModuleHandleA("engine.dll"), "A1 ? ? ? ? B9 ? ? ? ? FF 75 0C FF 75 08 FF 50 0C" + 0x1));
	//static viewmatrix& pMatrix = *reinterpret_cast<viewmatrix*>(0x210 * 2 + *(unsigned long*)((unsigned long)ff + 0xDC) - 0x44);
	//return pMatrix;




	
}

void c_renderer::refresh_viewport()
{
	this->dev->GetViewport(&port);
}

void c_renderer::init_device_objects(IDirect3DDevice9* _dev)
{
	dev = _dev;
	dev->GetViewport(&port);

	for (auto& font : fonts)
	{
		font.second.init_device_objects(dev);
		font.second.restore_device_objects();
	}
}

void c_renderer::invalidate_device_objects()
{
	dev = nullptr;

	for (auto& font : fonts)
		font.second.invalidate_device_objects();

	for (auto& handler : reset_handlers)
		handler();
}

void c_renderer::setup_render_state() const
{
	dev->SetVertexShader(nullptr);
	dev->SetPixelShader(nullptr);
	dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE); // NOLINT
	dev->SetRenderState(D3DRS_LIGHTING, FALSE);
	dev->SetRenderState(D3DRS_FOGENABLE, FALSE);
	dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	dev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	dev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	dev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	dev->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	dev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);

	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	dev->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
	dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dev->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
	dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dev->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

	dev->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
	dev->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN // NOLINT NOLINTNEXTLINE
		| D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);

	dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
}

void c_renderer::register_reset_handler(const std::function<void()> handler)
{
	reset_handlers.push_back(handler);
}

void c_renderer::build_lookup_table()
{
	if (!lookup_table.empty())
		return;

	for (auto i = 0; i <= points; i++)
		lookup_table.emplace_back(
		std::cos(2.f * D3DX_PI * (i / static_cast<float>(points))),
		std::sin(2.f * D3DX_PI * (i / static_cast<float>(points)))
		);

	for (auto lat = 0; lat < points_sphere_latitude; lat++)
	{
		const auto a1 = D3DX_PI * static_cast<float>(lat + 1) / (points_sphere_latitude + 1);
		const auto sin1 = sin(a1);
		const auto cos1 = cos(a1);

		for (auto lon = 0; lon <= points_sphere_longitude; lon++)
		{
			const auto a2 = 2 * D3DX_PI * static_cast<float>(lon) / points_sphere_longitude;
			const auto sin2 = sin(a2);
			const auto cos2 = cos(a2);

			lookup_sphere.emplace_back(sin1 * cos2, cos1, sin1 * sin2);
		}
	}
}
