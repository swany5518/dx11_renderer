#pragma once

#include <windowsx.h>
#include <type_traits>
#include <string>
#include <iostream>
#include <algorithm>

#include "renderer_utils.hpp"
#include "renderer.h"

//
// widget style structs and enums
//
struct checkbox_style
{
	// text styling
	float text_size;
	float text_outline_thickness;
	color text_color;
	color text_outline_color;
	color text_background_color;
	vec2 label_relative_pos; // labels position relative to the top left of the widget

	// border styling
	float border_thickness;
	float border_outline_thickness;
	color border_color;
	color border_outline_color;

	// checkbox checked rect styling
	color top_left_color;
	color top_right_color;
	color bottom_left_color;
	color bottom_right_color;
};

struct button_style
{
	// text styling
	float text_size;
	float text_outline_thickness;
	color text_color;
	color text_outline_color;
	color text_background_color;

	// border styling
	float border_thickness;
	float border_outline_thickness;
	color border_color;
	color border_outline_color;

	// button coloring
	color top_left_color;
	color top_right_color;
	color bottom_left_color;
	color bottom_right_color;
};

struct slider_style
{
	// text styling
	float text_size;
	float text_outline_thickness;
	color text_color;
	color text_outline_color;
	color text_background_color;
	vec2 label_relative_pos; // labels position relative to the top left of the widget

	// border styling
	float border_thickness;
	float border_outline_thickness;
	color border_color;
	color border_outline_color;

	// slide coloring
	color bg_color;
	color top_left_color;
	color top_right_color;
	color bottom_left_color;
	color bottom_right_color;
};

struct text_entry_style
{
	// text styling
	float text_size;
	float text_outline_thickness;
	color text_color;
	color text_outline_color;
	color text_background_color;
	vec2 label_relative_pos; // labels position relative to the top left of the widget

	// border styling
	float border_thickness;
	float border_outline_thickness;
	color border_color;
	color border_outline_color;

	// entry box_color
	color background_color;
	color top_left_color;
	color top_right_color;
	color bottom_left_color;
	color bottom_right_color;
};

struct combo_box_style
{
	// text styling
	float text_size;
	float text_offset;
	float text_outline_thickness;
	color text_color;
	color text_outline_color;
	color text_background_color;

	// border styling
	float border_thickness;
	float border_outline_thickness;
	color border_color;
	color border_outline_color;

	// interior coloring
	color top_left_color;
	color top_right_color;
	color bottom_left_color;
	color bottom_right_color;
};

//
// widget utilities
//

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

// union for widgets to know if they are clicked, clicking, dragging or hovered
struct mouse_state
{
	vec2 click_origin; // used for dragging widgets

	union
	{
		uint8_t state;	   // contains various mouse information

		struct
		{
			uint8_t clicked : 1;
			uint8_t clicking : 1;
			uint8_t hovering : 1;
			uint8_t reserved : 5;
		};
	};

	mouse_state(const vec2& click_origin) :
		click_origin(click_origin),
		state(0)
	{ }
};

//
// widgets 
//

// base widget inheritable, includes size and position and click/clicking/hovered state
struct widget
{
	vec2 top_left;
	vec2 size;
	std::wstring label;
	mouse_state mouse_info;
	bool active;

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
		mouse_info({ 0.f, 0.f }),
		active(true)
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
	virtual void on_lbutton_down(const vec2& mouse_position) 
	{ 
		mouse_info.clicking = 1;
		mouse_info.clicked = 0;
		mouse_info.hovering = 0;
		mouse_info.click_origin = relative_position(mouse_position);
	}
	virtual void on_lbutton_up(const vec2& mouse_position) 
	{ 
		if (mouse_info.clicking)
		{
			mouse_info.clicking = 0;
			mouse_info.clicked = 1;
		}
	}
	virtual void on_drag(const vec2& new_position) {}
	virtual void on_widget_move(const vec2& new_position)
	{
		top_left = new_position - mouse_info.click_origin;
	}

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
	checkbox_style* style;

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
	button_style* style;

	button() = delete;

	button(const vec2& top_left, const vec2& size, const std::wstring& label, button_style* style) :
		widget(top_left, size, label),
		style(style)
	{ }
	
	void draw()
	{
		// add button rect
		p_renderer->add_rect_filled_multicolor(top_left, size, style->top_left_color, style->top_right_color, style->bottom_left_color, style->bottom_right_color);

		// add button border
		p_renderer->add_outlined_frame(top_left, size, style->border_thickness, style->border_outline_thickness, style->border_color, style->border_outline_color);

		// add button text
		p_renderer->add_outlined_text_with_bg(top_left, size, label, style->text_color, style->text_outline_color, style->text_background_color, style->text_size, style->text_outline_thickness, text_align::center_middle);
		
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
	slider_style* style;

	slider(const vec2& top_left, const vec2& size, const std::wstring& label, Ty* value, Ty min, Ty max, slider_style* style) :
		widget(top_left, size, label),
		value(value),
		min_value(min),
		max_value(max),
		style(style)
	{ }

	Ty get_range()
	{
		return max_value - min_value;
	}

	void on_lbutton_down(const vec2& mouse_position)
	{
		mouse_info.clicking = 1;
		mouse_info.clicked = 0;
		mouse_info.hovering = 0;
		mouse_info.click_origin = relative_position(mouse_position);

		if (active)
		{
			float ratio = std::clamp<float>(mouse_position.x - top_left.x, 0.f, size.x) / size.x;

			if (std::is_integral_v<Ty>)
				*value = static_cast<Ty>(std::round(static_cast<float>(get_range()) * ratio + static_cast<float>(min_value)));
			else if (std::is_floating_point_v<Ty>)
				*value = static_cast<Ty>(get_range() * ratio + min_value);
		}
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
		// calculate the slider width
		float scaled_width = static_cast<float>(*value) / static_cast<float>(get_range()) * size.x;

		// add the background color
		p_renderer->add_rect_filled(top_left, size, style->bg_color);

		// add the slider
		p_renderer->add_rect_filled_multicolor(top_left, { scaled_width, size.y }, style->top_left_color, style->top_right_color, style->bottom_left_color, style->bottom_right_color);

		// add the border
		p_renderer->add_outlined_frame(top_left, size, style->border_thickness, style->border_outline_thickness, style->border_color, style->border_outline_color);

		// add the label
		p_renderer->add_outlined_text_with_bg(top_left + style->label_relative_pos, size, label, style->text_color, style->text_outline_color, style->text_background_color, style->text_size, style->text_outline_thickness);
		
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
	text_entry_style* style;

	text_entry(const vec2& top_left, const vec2& size, const std::wstring& label, text_entry_style* style, uint32_t max_buffer_size = 128u) :
		widget(top_left, size, label),
		buffer(),
		style(style),
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
	combo_box_style* style;

	combo_box(const vec2& top_left, const vec2& size, const std::wstring& label, combo_box_style* style) :
		widget(top_left, size, label),
		style(style)
	{ }

	void draw()
	{
		p_renderer->add_outlined_frame(top_left, size, style->border_thickness, style->border_outline_thickness, style->border_color, style->border_outline_color );
		p_renderer->add_outlined_text_with_bg({ top_left.x + style->text_offset, top_left.y - style->text_size / 2.f - style->text_outline_thickness }, size, label, style->text_color, style->text_outline_color, style->text_background_color, style->text_size);
	}

	void on_drag(const vec2& new_position)
	{
		//top_left = new_position - mouse_info.click_origin; //- relative_position(new_position);
	}

	widget_type get_type()
	{
		return widget_type::combo_box;
	}
};

  /////////////////////////////////////////////
 // debugging stuff, delete/make nice later //
/////////////////////////////////////////////
inline static combo_box_style combo_box_style_default{ 14.f, 20.f, 1.f, {1.f, 1.f, 1.f, 1.f}, {0.f, 0.f, 0.f, 1.f}, {0.f, 0.f, 0.f, 0.f}, 2.f, 1.5f, {1.f, 1.f, 1.f, 1.f}, {.75f, .75f, .75f, .75f}, {0.f, 0.f, 0.f, 0.f}, {0.f, 0.f, 0.f, 0.f}, {0.f, 0.f, 0.f, 0.f}, {0.f, 0.f, 0.f, 0.f} };
inline static button_style button_style_default{14.f, 1.f, {1.f, 1.f, 1.f, 1.f}, {0.f, 0.f, 0.f, 1.f}, {0.f, 0.f, 0.f, 0.f}, 2.f, 1.5f, {1.f, 1.f, 1.f, 1.f}, {.75f, .75f, .75f, .75f}, {0.f, 0.f, 0.f, 0.f}, {0.f, 0.f, 0.f, 0.f}, {0.f, 0.f, 0.f, 0.f}, {0.f, 0.f, 0.f, 0.f} };
inline static slider_style slider_style_default{ 14.f, 1.f, {1.f, 1.f, 1.f, 1.f}, {0.f, 0.f, 0.f, 1.f}, {0.f, 0.f, 0.f, 0.f}, {0, 0}, 2.f, 1.5f, {1.f, 1.f, 1.f, 1.f}, {.75f, .75f, .75f, .75f}, {0.f, 0.f, 0.f, 0.f}, {0.f, 0.f, 0.f, 1.f}, {0.f, 0.f, 0.f, 1.f}, {0.f, 0.f, 0.f, 1.f}, {0.f, 0.f, 0.f, 1.f} };

inline static float test = 5.f;
inline static float test2 = 5.f;

inline static slider<float> slide{ vec2{ 100.f, 100.f }, { 200, 20 }, L"slider",  &test, 0.f, 20.f, &slider_style_default };
inline static slider<float> slide1{ vec2{ 100.f, 130.f }, { 300, 20 }, L"slider", &test2, 0.f, 20.f, &slider_style_default };
inline static button btn{ {400, 400}, {100, 20}, L"button", &button_style_default};
inline static combo_box cb{ {200, 200}, {300, 400}, L"combo box", &combo_box_style_default };

inline static std::vector<widget*> widgets =
{
	&slide,
	&slide1,
	&cb,
	&btn
};

inline bool debug_move = false;

inline static void widget_msg_handler(UINT message, WPARAM w_param, LPARAM l_param)
{
	vec2 pos;
	switch (message)
	{
	case WM_LBUTTONDOWN:
		//std::cout << "LBUTTON DOWN" << std::endl;
		pos = { GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) };

		for (auto w : widgets)
		{
			if (w->contains(pos))
			{
				w->active = !debug_move;
				w->on_lbutton_down(pos);
				break;
			}
		}
			
		break;

	case WM_LBUTTONUP:
		//std::cout << "LBUTTON UP" << std::endl;
		for (auto w : widgets)
		{
			if (w->contains(pos))
				w->on_lbutton_up(pos);

			w->mouse_info.click_origin = { 0, 0 };
			w->mouse_info.clicking = false;
		}

		break;
	case WM_MOUSEMOVE:
		pos = { GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) };
		//std::cout << "MOUSEMOVE X: " << pos.x << " Y: " << pos.y << std::endl;

		for (auto w : widgets)
		{
			if (w->mouse_info.clicking)
			{
				if (debug_move)
					w->on_widget_move(pos);
				else
					w->on_drag(pos);
			}
		}

		break;
	}
}