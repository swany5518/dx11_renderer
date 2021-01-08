#pragma once

#include <type_traits>
#include <string>
#include <iostream>
#include <algorithm>

#include "renderer_utils.hpp"
#include "renderer.h"


// enum class containing all possible types of widgets
enum class widget_type
{
	unknown,
	checkbox,
	button,
	slider,
	text_entry,
	combo_box
};

// union for widgets to know if they are clicked, clicking or hovered
union mouse_state
{
	uint8_t state;

	struct
	{
		uint8_t clicked  : 1;
		uint8_t clicking : 1;
		uint8_t hovering : 1;
		uint8_t reserved : 5;
	};
};

// base widget inheritable, includes size and position and click/clicking/hovered state
struct widget
{
	vec2 top_left;
	vec2 size;
	std::wstring label;
	mouse_state mouse_info;

	inline static renderer* p_renderer;

	inline static void set_renderer(renderer* p_instance)
	{
		p_renderer = p_instance;
	}

	widget() = delete;

	widget(const vec2& top_left, const vec2& size, const std::wstring& label) : 
		top_left(top_left),
		size(size),
		label(label),
		mouse_info()
	{ }

	// determine if an widget contains a position
	bool contains(const vec2& pos)
	{
		return pos.x >= top_left.x
			&& pos.x <= top_left.x + size.x
			&& pos.y >= top_left.y
			&& pos.y <= top_left.y + size.y;
	}

	// get relative position from the top_left of the widget
	vec2 relative_position(const vec2& pos)
	{
		return pos - top_left;
	}

	// virtual drawing function
	virtual void draw() {  }

	// should be called when the mouse is over the widget
	virtual void on_lbutton_down() 
	{ 
		mouse_info.clicking = 1;
		mouse_info.clicked = 0;
		mouse_info.hovering = 0;
		
	}
	virtual void on_lbutton_up() 
	{ 
		mouse_info.clicking = 0;
		mouse_info.clicked = 1;
		mouse_info.hovering = 1;
	}
	virtual void on_drag(const vec2& new_position) {};

	// virtual widget type function
	virtual widget_type get_type()
	{
		return widget_type::unknown;
	}
};

// simple checkbox widget for boolean values
struct checkbox : widget
{
	bool* value;

	checkbox() = delete;

	checkbox(const vec2& top_left, const vec2& size, const std::wstring& label, bool* value) :
		widget{ top_left, size, label },
		value(value) 
	{ }

	void draw()
	{
		p_renderer->add_wire_frame(top_left, size, { 1.f, 0.f, 0.f, 1.f });

		if (*value)
			p_renderer->add_rect_filled_multicolor(
				vec2{ top_left.x + 2.f, top_left.y + 2.f },
				vec2{ size.x - 4.f, size.y - 4.f },
				color{ 1.f, 0.f, 0.f, 1.f },
				color{ 1.f, 0.f, 0.f, 1.f },
				color{ 1.f, 0.f, 0.f, 1.f },
				color{ 1.f, 0.f, 0.f, 1.f });
	}

	widget_type get_type()
	{
		return widget_type::checkbox;
	}
};

// simple button widget for event triggering
struct button : widget
{
	button() = delete;

	button(const vec2& top_left, const vec2& size, const std::wstring& label) :
		widget(top_left, size, label)
	{ }
	
	void draw()
	{
		p_renderer->add_rect_filled_multicolor(top_left, size, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f }, { 1.f, 1.f, 1.f, 1.f });
		p_renderer->add_text(top_left, size, label, { 1.f, 0.f, 0.f, 1.f }, 14.f, text_align::center_middle);
	}

	widget_type get_type()
	{
		return widget_type::button;
	}
};

// simple slider widget for floating point and integer types
template<typename Ty>
struct slider : widget
{
	Ty* value;
	Ty min_value;
	Ty max_value;

	slider(const vec2& top_left, const vec2& size, const std::wstring& label, Ty* value, Ty min, Ty max) :
		widget(top_left, size, label),
		value(value),
		min_value(min),
		max_value(max)
	{ 
		
	}

	Ty get_range()
	{
		return max_value - min_value;
	}

	void on_drag(const vec2& new_position)
	{
		// ratio is the clamped offset of the new position relative to the top_left.x / the slider's size.x
		float ratio = std::clamp<float>(new_position.x - top_left.x, 0.f, size.x) / size.x;
		
		if (std::is_integral_v<Ty>)
			*value = static_cast<Ty>(std::round(static_cast<float>(get_range()) * ratio + static_cast<float>(min_value)));
		else if (std::is_floating_point_v<Ty>)
			*value = static_cast<Ty>(get_range() * ratio + min_value);
	}

	void draw()
	{
		float scaled_width = static_cast<float>(*value) / static_cast<float>(get_range()) * size.x;
		p_renderer->add_rect_filled_multicolor(top_left, { scaled_width, size.y }, { 1.f, 0.f, 0.f, 1.f }, { 1.f, 0.f, 0.f, 1.f }, { 1.f, 0.f, 0.f, 1.f }, { 1.f, 0.f, 0.f, 1.f });
		p_renderer->add_frame(top_left, size, 2.f, { 1.f, 1.f, 1.f, 1.f });
		
	}

	widget_type get_type()
	{
		return widget_type::slider;
	}
};

// simple text entry
struct text_entry : widget
{
	std::string buffer;
	uint32_t max_buffer_size;

	text_entry(const vec2& top_left, const vec2& size, const std::wstring& label, uint32_t max_buffer_size = 128u) :
		widget(top_left, size, label),
		buffer(),
		max_buffer_size(max_buffer_size)

	{ 
		buffer.reserve(max_buffer_size);
	}

	void draw()
	{

	}

	widget_type get_type()
	{
		return widget_type::text_entry;
	}
};

// widget commonly used to place multiple grouped widgets inside
struct combo_box : widget
{
	void draw()
	{

	}

	widget_type get_type()
	{
		return widget_type::combo_box;
	}
};