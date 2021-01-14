#pragma once

#include "widget_utils.h"

mouse_state::mouse_state(const vec2& click_origin) : 
	click_origin(click_origin),
	state(0)
{ }