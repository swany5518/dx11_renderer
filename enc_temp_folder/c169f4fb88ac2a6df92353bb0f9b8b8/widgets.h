#pragma once

#include <windowsx.h>
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
		mouse_info({ 0.f, 0.f })
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
	combo_box(const vec2& top_left, const vec2& size, const std::wstring& label) :
		widget(top_left, size, label)
	{ }

	void draw()
	{
		float text_offset = 20.f;
		float text_size = 14.f;
		float thickness = 2.f;
		float shadow_thickness = 1.5f;
		color frame_color{ .75f, .75f, .75f, 1.f };
		color outline_color{ .2f, .2f, .2f, .8f };
		color text_color{ 1.f, 1.f, 1.f, 1.f };
		color text_outline{ 0.f, 0.f, 0.f, 1.f };
		color wnd_bg{ 0.f, 0.f, 0.5f, 1.f };

		p_renderer->add_outlined_frame(top_left, size, thickness, shadow_thickness, frame_color, outline_color );
		//p_renderer->add_text_with_background({ top_left.x + text_offset, top_left.y - text_size / 2.f - shadow_thickness }, size, label, text_color, wnd_bg, text_size, text_align::left_top);
		p_renderer->add_outlined_text_with_bg({ top_left.x + text_offset, top_left.y - text_size / 2.f - shadow_thickness }, size, label, text_color, text_outline, wnd_bg, text_size);
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



// debugging stuff, delete later
inline static float test = 5.f;
inline static float test2 = 5.f;

inline static slider<float> slide{ vec2{ 100.f, 100.f }, { 200, 20 }, L"slider",  &test, 0.f, 20.f };
inline static slider<float> slide1{ vec2{ 100.f, 130.f }, { 300, 20 }, L"slider", &test2, 0.f, 20.f };
inline static combo_box cb{ {200, 200}, {300, 400}, L"combo box" };

inline static std::vector<widget*> widgets =
{
	&slide,
	&slide1,
	&cb
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
				//std::cout << "setting clicking to true from wndproc" << std::endl;
				w->mouse_info.click_origin = w->relative_position(pos);
				w->mouse_info.clicking = true;
				break;
			}
		}

		break;

	case WM_LBUTTONUP:
		//std::cout << "LBUTTON UP" << std::endl;
		for (auto w : widgets)
		{
			if (w->mouse_info.clicking && w->contains(pos))
				w->mouse_info.clicked = true;

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