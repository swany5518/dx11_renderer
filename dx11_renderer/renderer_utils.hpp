#pragma once

#include "../FW1FontWrapper/Source/FW1FontWrapper.h"

#define PI 3.141592654f

// struct for 2d position
struct vec2
{
	float x, y;

	vec2() :
		x(0.f), y(0.f)
	{ }

	vec2(float x, float y) :
		x(x), y(y)
	{ }

	vec2(int x, int y) :
		x(static_cast<float>(x)),
		y(static_cast<float>(y))
	{ }

	bool operator==(const vec2& other) const
	{
		return x == other.x && y == other.y;
	}

	vec2 operator-(const vec2& other) const
	{
		return { x - other.x, y - other.y };
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

// text formatting flags
enum class text_flags
{
	left_align   = FW1_LEFT,
	center_align = FW1_CENTER,
	right_align  = FW1_RIGHT
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

// function for calculating a circles vertex position
inline float calc_theta(int vertex_index, size_t total_points)
{
	return 2.f * PI * static_cast<float>(vertex_index) / static_cast<float>(total_points);
}