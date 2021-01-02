#pragma once

#include <d3d11.h>
#include <cstdint>
#include <cstddef>
#include <cmath>
#include <vector>
#include <string>
#include <unordered_map>
#include <cassert>
#include <D3DX10.h>
#include <D3DX11.h>
#include <DirectXMath.h>
#include "../FW1FontWrapper/Source/FW1FontWrapper.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")
#pragma warning(disable:6387)

#include "shaders.hpp"

#define MAX_DRAW_LIST_VERTICES 0x10000
#define PI 3.141592654f

// struct for 2d position
struct vec2
{
	float x, y;

	vec2() :
		x(0.f), y(0.f)
	{}

	vec2(float x, float y) :
		x(x), y(y)
	{}

	bool operator==(const vec2& other) const
	{
		return x == other.x && y == other.y;
	}

	// higher y corresponds to a smaller x value
	bool higher_or_leftmost(const vec2& other) const
	{
		return y < other.y || y == other.y && x < other.x;
	}

	bool rightmost_or_higher(const vec2& other) const
	{
		return x > other.x || x == other.x && y < other.y;
	}
};

// struct for 3d position
struct vec3
{
	float x, y, z;

	vec3(float x, float y, float z) :
		x(x), y(y), z(z)
	{}
};

// struct for rgba colors
struct color
{
	float r, g, b, a;

	color(float r, float g, float b, float a) :
		r(r), g(g), b(b), a(a)
	{}

	void operator+=(float amount)
	{
		r += amount;
		g += amount;
		b += amount;
	}

	// convert float 4 rgba to uint32 hex abgr
	uint32_t to_hex_abgr() const
	{
		uint32_t hex{};
		hex |= static_cast<uint32_t>(a * 255) << 24;
		hex |= static_cast<uint32_t>(b * 255) << 16;
		hex |= static_cast<uint32_t>(g * 255) << 8;
		hex |= static_cast<uint32_t>(r * 255) << 0;

		return hex;
	}
};

// a struct that contains position and color information that the gpu will process
struct vertex
{
	float x, y, z;
	float r, g, b, a;

	vertex() :
		x(0.f), y(0.f), z(0.f), 
		r(0.f), g(0.f), b(0.f), a(0.f)
	{}

	vertex(float x, float y, float z, float r, float g, float b, float a) :
		x(x), y(y), z(z),
		r(r), g(g), b(b), a(a)
	{}

	vertex(const vec2& pos, const color& rgba) :
		x(pos.x), y(pos.y), z(1.f),
		r(rgba.r), g(rgba.g), b(rgba.b), a(rgba.a)
	{}

	vertex(const vec3& pos, const color& rgba) :
		x(pos.x), y(pos.y), z(pos.z),
		r(rgba.r), g(rgba.g), b(rgba.b), a(rgba.a)
	{}

	vertex(float x, float y, float z, const color& rgba) :
		x(x), y(y), z(z), 
		r(rgba.r), g(rgba.g), b(rgba.b), a(rgba.a)
	{}
	
	void operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
	}

	void operator+=(float addition)
	{
		x += addition;
		y += addition;
	}

	void operator+=(const vec2& add)
	{
		x += add.x;
		y += add.y;
	}
};

// a struct that contains counts and primitive topology type for a vertex or vertices
struct batch
{
	batch(D3D_PRIMITIVE_TOPOLOGY type, uint32_t vertex_count) : 
		type(type),
		vertex_count(vertex_count)
	{}

	D3D_PRIMITIVE_TOPOLOGY type;
	uint32_t vertex_count;
};

// function for safely releasing com object pointers
template <typename T>
inline void safe_release(T com_ptr)
{
	static_assert(std::is_pointer<T>::value, "safe_release - invalid com_ptr");
	static_assert(std::is_base_of<IUnknown, std::remove_pointer<T>::type>::value, "safe_release - com_ptr not a com object");
	if (com_ptr)
	{
		com_ptr->Release();
		com_ptr = 0;
	}
}

// holds a vertex buffer and a batch list that our renderer will use
class draw_list
{
	friend class renderer;
public:
	draw_list() :
		vertices(),
		batch_list(),
		p_text_geometry(nullptr)
	{}

	void clear()
	{
		vertices.clear();
		batch_list.clear();
		p_text_geometry->Clear();
	}

	HRESULT init_text_geometry(IFW1Factory* font_factory)
	{
		return font_factory->CreateTextGeometry(&p_text_geometry);
	}

	~draw_list()
	{
		safe_release(p_text_geometry);
	}

private:
	std::vector<vertex> vertices;
	std::vector<batch> batch_list;
	IFW1TextGeometry* p_text_geometry;
};

// provides a directx api to easily render primitives
class renderer
{
public:
	// create a renderer to draw on the given window
	renderer(HWND hwnd);
	// testing constructor, remove later or refactor nicely
	renderer(HWND hwnd, bool should_use_pixel_cords);
	~renderer();

	// submits the draw list to the gpu for rendering
	void draw();

	// adds a colored line from start to end
	void add_line(const vec2& start, const vec2& end, const color& color);
	
	// adds a connected line from passed in points
	void add_polyline(const vec2* points, size_t size, const color& color);

	// adds a multicolored line from start to end
	void add_line_multicolor(const vec2& start, const color& start_color, const vec2& end, const color& end_color);
	
	// add a rectangle 
	void add_rect_filled(const vec2& top_left, const vec2& size, const color& color);
	
	// add a multicolored rectangle
	void add_rect_filled_multicolor(const vec2& top_left, const vec2& size, const color& top_left_color, const color& top_right_color, const color& bottom_left_color, const color& bottom_right_color);
	
	// add triangle
	void add_triangle(const vec2& p1, const vec2& p2, const vec2& p3, const color& color);
	
	// add a filled triangle, vertices get arranged to clockwise order
	void add_triangle_filled(const vec2& p1, const vec2& p2, const vec2& p3, const color& color);
	
	// add a multicolored triangle, vertices get arranged to clockwise order so colors might not be on expected points
	void add_triangle_filled_multicolor(const vec2& p1, const vec2& p2, const vec2& p3, const color& p1_color, const color& p2_color, const color& p3_color);
	
	// add a circle, more segments means smoother looking circle
	void add_circle(const vec2& middle, float radius, const color& color, size_t segments);
	
	// add a filled circle
	void add_circle_filled(const vec2& middle, float radius, const color& box_color, size_t segments);

	// add a thin frame made out of rects
	void add_frame(const vec2& top_left, const vec2& size, float thickness, const color& frame_color);

	// add a wire frame (constructs frame from lines instead of rects)
	void add_wire_frame(const vec2& top_left, const vec2& size, const color& frame_color);

	// add a 3d wire frame
	void add_3d_wire_frame(const vec2& top_left, const vec3& size, const color& frame_color);

	// add an outlined frame
	void add_outlined_frame(const vec2& top_left, const vec2& size, float thickness, float outline_thickness, const color& color_, const color& outline_color);

	// add text 
	void add_text(const vec2& pos, const std::wstring& text, const color& color, float font_size, uint32_t text_flags = NULL);

private:
	IDXGISwapChain*			p_swapchain;      // swapchain ptr
	ID3D11Device*			p_device;         // d3d device interface ptr
	ID3D11DeviceContext*	p_device_context; // d3d device context ptr
	ID3D11RenderTargetView* p_backbuffer;     // backbuffer ptr
	ID3D11InputLayout*		p_layout;         // layout ptr
	ID3D11VertexShader*		p_vertex_shader;  // vertex shader ptr
	ID3D11PixelShader*		p_pixel_shader;   // pixel shader ptr
	ID3D11Buffer*			p_vertex_buffer;  // vertex buffer ptr
	ID3D11Buffer*			p_screen_projection_buffer; // screen projection buffer ptr

	IFW1Factory*			p_font_factory; // font factory ptr
	IFW1FontWrapper*		p_font_wrapper;  // font wrapper ptr
	const std::wstring		font_family = L"Arial";

	draw_list default_draw_list; // default draw list, we should only need 1 draw list. In the future we could add more
	DirectX::XMMATRIX screen_projection;

	// add a vertex to the draw list
	void add_vertex(const vertex& vertex, const D3D_PRIMITIVE_TOPOLOGY type);

	// adds multiple vertices of the same typr to the defualt draw list
	void add_vertices(const vertex* p_vertices, const size_t vertex_count, const D3D_PRIMITIVE_TOPOLOGY type);

	// process errors coming from the renderer
	void handle_error(const char* );
};

// function for calculating a circles vertex position
inline float calc_theta(int vertex_index, size_t total_points)
{
	return 2.f * PI * static_cast<float>(vertex_index) / static_cast<float>(total_points);
}