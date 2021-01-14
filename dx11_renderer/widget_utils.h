#pragma once

#include <cstdint>

#include "renderer_utils.h"

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
	combo_box,
	color_picker
};

// enum class containing all mouse cursors 
enum class mouse_cursor
{
	arrow,
	text_input,
	resize_all,
	resize_ew,
	resize_ns,
	resize_nesw,
	resize_nwse,
	hand,
	not_allowed
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
			uint8_t clicked : 1; // if a widget was clicked
			uint8_t clicking : 1; // if a widget is being clicked
			uint8_t hovering : 1; // if a widget is being hovered
			uint8_t reserved : 5;
		};
	};

	mouse_state(const vec2&);
};
