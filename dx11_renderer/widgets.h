#pragma once

#include <type_traits>
#include <string>

#include "renderer_utils.hpp"
#include "renderer.h"
#include <iostream>

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

// base widget inheritable, includes size and position and click/clicking/hovered state
struct widget
{
	vec2 top_left;
	vec2 size;
	std::string label;
	bool has_focus;
	bool was_clicked;
	bool is_clicking;
	bool is_hovered;

	inline static renderer* p_renderer;

	inline static void set_renderer(renderer* p_instance)
	{
		p_renderer = p_instance;
	}

	widget() = delete;

	widget(const vec2& top_left, const vec2& size, const std::string& label) : 
		top_left(top_left),
		size(size),
		label(label),
		has_focus(false),
		was_clicked(false),
		is_clicking(false),
		is_hovered(false)
	{ }

	// determine if an interactible contains a position
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

	// virtual drawing function that widgets should override
	virtual void draw() { std::cout << "draw called from base" << std::endl; }

	virtual void on_lbutton_down() { std::cout << "lmao down from base" << std::endl; }
	virtual void on_lbutton_up() { std::cout << "lmao up from base" << std::endl; }

	// virtual eidget type function that widgets should override
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

	checkbox(const vec2& top_left, const vec2& size, const std::string& label, bool* value) :
		widget{ top_left, size, label },
		value(value) 
	{ }

	void on_lbutton_down()
	{
		if (has_focus)
			std::cout << "we have focus" << std::endl;
		std::cout << "lmao down" << std::endl;
		has_focus = true;

		if (has_focus)
			std::cout << "we have focus now" << std::endl;
	}

	void on_lbutton_up()
	{
		std::cout << "lmao up" << std::endl;
		has_focus = false;
		*value = !*value;
	}

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

	button(const vec2& top_left, const vec2& size, const std::string& label) :
		widget(top_left, size, label)
	{ }

	void draw()
	{

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

	slider(const vec2& top_left, const vec2& size, const std::string& label, Ty value, Ty min, Ty max) :
		widget(top_left, size, label),
		value(value),
		min_value(min),
		max_value(max)
	{ }

	Ty get_range()
	{
		return max_value - min_value;
	}

	void draw()
	{

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

	text_entry(const vec2& top_left, const vec2& size, const std::string& label, uint32_t max_buffer_size = 128u) :
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