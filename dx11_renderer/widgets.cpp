#include "widgets.h"

//
// widget definitions
//

widget::widget(const vec2& top_left, const vec2& size, const std::wstring& label) :
	top_left(top_left),
	size(size),
	label(label),
	label_pos(size.x, 0.f),
	mouse_info({ 0.f, 0.f }),
	active(true)
{ }

widget::widget(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos) :
	top_left(top_left),
	size(size),
	label(label),
	label_pos(label_pos),
	mouse_info({ 0.f, 0.f }),
	active(true)
{ }

bool widget::contains(const vec2& pos)
{
	return pos.x >= top_left.x
		&& pos.x <= top_left.x + size.x
		&& pos.y >= top_left.y
		&& pos.y <= top_left.y + size.y;
}

vec2 widget::relative_position(const vec2& pos)
{
	return pos - top_left;
}

void widget::draw() 
{ }

void widget::on_lbutton_down(const vec2& mouse_position)
{
	mouse_info.clicking = 1;
	mouse_info.clicked = 0;
	mouse_info.hovering = 0;
	mouse_info.click_origin = relative_position(mouse_position);
}

void widget::on_lbutton_up(const vec2& mouse_position)
{
	if (mouse_info.clicking)
	{
		mouse_info.clicking = 0;
		mouse_info.clicked = 1;
	}
}

void widget::on_drag(const vec2& new_position) 
{ 
	std::cout << "base on drag" << std::endl; 
}

void widget::on_widget_move(const vec2& new_position)
{
	top_left = new_position - mouse_info.click_origin;
}

void widget::on_key_down(char key) 
{ 

}

widget_type widget::get_type()
{
	return widget_type::unknown;
}

//
// checkbox definitions
//

checkbox::checkbox(const vec2& top_left, const vec2& size, const std::wstring& label, bool* value, checkbox_style* style) :
	widget( top_left, size, label),
	value(value),
	style(style)
{ }

checkbox::checkbox(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos, bool* value, checkbox_style* style) :
	widget( top_left, size, label, label_pos),
	value(value),
	style(style)
{ }

void checkbox::on_lbutton_up(const vec2& mouse_position)
{
	if (mouse_info.clicking)
	{
		*value = !*value;
		mouse_info.clicked = true;
		mouse_info.clicking = false;;
	}
}

void checkbox::draw()
{
	// add checkbox background
	p_renderer->add_rect_filled_multicolor(top_left, size, style->bg.tl_clr, style->bg.tr_clr, style->bg.bl_clr, style->bg.br_clr);

	// add checkbox border
	p_renderer->add_outlined_frame(top_left, size, style->border.thckns, style->border.ol_thckns, style->border.clr, style->border.ol_clr);

	// add label
	p_renderer->add_outlined_text_with_bg(top_left + label_pos, size, label, style->text.clr, style->text.ol_clr, style->text.bg_clr, style->text.size, style->text.ol_thckns);

	// if value is true, add rect inside
	if (*value)
		p_renderer->add_rect_filled_multicolor(top_left + label_pos, size - (style->gap * 2.f), style->check.tl_clr, style->check.tr_clr, style->check.bl_clr , style->check.br_clr);
}

widget_type checkbox::get_type()
{
	return widget_type::checkbox;
}

//
// button definitions
//

button::button(const vec2& top_left, const vec2& size, const std::wstring& label, button_style* style) :
	widget(top_left, size, label),
	style(style)
{ }

button::button(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos, button_style* style) :
	widget(top_left, size, label, label_pos),
	style(style)
{ }

void button::draw()
{
	// add button rect
	p_renderer->add_rect_filled_multicolor(top_left, size, style->bg.tl_clr, style->bg.tr_clr, style->bg.bl_clr, style->bg.br_clr);

	// add button border
	p_renderer->add_outlined_frame(top_left, size, style->border.thckns, style->border.ol_thckns, style->border.clr, style->border.ol_clr);

	// add button text
	p_renderer->add_outlined_text_with_bg(top_left + label_pos, size, label, style->text.clr, style->text.ol_clr, style->text.bg_clr, style->text.size, style->text.ol_thckns, text_align::center_middle);
}

widget_type button::get_type()
{
	return widget_type::button;
}

//
// slider definitions
//

template<typename Ty>
slider<Ty>::slider(const vec2& top_left, const vec2& size, const std::wstring& label, Ty* value, Ty min, Ty max, slider_style* style) :
	widget(top_left, size, label),
	value(value),
	min_value(min),
	max_value(max),
	style(style)
{ }

template<typename Ty>
slider<Ty>::slider(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos, Ty* value, Ty min, Ty max, slider_style* style) :
	widget(top_left, size, label, label_pos),
	value(value),
	min_value(min),
	max_value(max),
	style(style)
{ }

template<typename Ty>
Ty slider<Ty>::get_range()
{
	return max_value - min_value;
}

template<typename Ty>
void slider<Ty>::on_lbutton_down(const vec2& mouse_position)
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

template<typename Ty>
void slider<Ty>::on_drag(const vec2& new_position)
{
	// ratio is the clamped offset of the new position relative to the top_left.x / the slider's size.x
	float ratio = std::clamp<float>(new_position.x - top_left.x, 0.f, size.x) / size.x;

	if (std::is_integral_v<Ty>)
		*value = static_cast<Ty>(std::round(static_cast<float>(get_range()) * ratio + static_cast<float>(min_value)));
	else if (std::is_floating_point_v<Ty>)
		*value = static_cast<Ty>(get_range() * ratio + min_value);
}

template<typename Ty>
void slider<Ty>::draw()
{
	// calculate the slider width
	float scaled_width = static_cast<float>(*value - min_value) / static_cast<float>(get_range()) * size.x;

	// add the background color
	p_renderer->add_rect_filled_multicolor(top_left, size, style->bg.tl_clr, style->bg.tr_clr, style->bg.bl_clr, style->bg.br_clr);

	// add the slider
	p_renderer->add_rect_filled_multicolor(top_left, { scaled_width, size.y }, style->clr.tl_clr, style->clr.tr_clr, style->clr.bl_clr, style->clr.br_clr);

	// add the border
	p_renderer->add_outlined_frame(top_left, size, style->border.thckns, style->border.ol_thckns, style->border.clr, style->border.ol_clr);

	// add the label
	p_renderer->add_outlined_text_with_bg(top_left + label_pos, size, label, style->text.clr, style->text.ol_clr, style->text.bg_clr, style->text.size, style->text.ol_thckns);
}

template<typename Ty>
widget_type slider<Ty>::get_type()
{
	return widget_type::slider;
}

//
// text entry definitions
//

text_entry::text_entry(const vec2& top_left, const vec2& size, const std::wstring& label, text_entry_style* style, uint32_t max_buffer_size, const vec2& buffer_offset) :
	widget(top_left, size, label),
	buffer(),
	buffer_offset(buffer_offset),
	max_buffer_size(max_buffer_size),
	style(style)
{
	buffer.reserve(max_buffer_size);
}

text_entry::text_entry(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos, text_entry_style* style, uint32_t max_buffer_size, const vec2& buffer_offset) :
	widget(top_left, size, label, label_pos),
	buffer(),
	buffer_offset(buffer_offset),
	max_buffer_size(max_buffer_size),
	style(style)
{
	buffer.reserve(max_buffer_size);
}

void text_entry::on_key_down(char key)
{
	switch (key)
	{
	case 0x08: // backspace
		if (buffer.size())
			buffer.pop_back();
		break;
	default:
		if (buffer.size() < max_buffer_size)
			buffer += key;
	}
}

void text_entry::draw()
{
	// add our text entry background
	p_renderer->add_rect_filled_multicolor(top_left, size, style->bg.tl_clr, style->bg.tr_clr, style->bg.bl_clr, style->bg.br_clr);

	// add our border
	p_renderer->add_outlined_frame(top_left, size, style->border.thckns, style->border.ol_thckns, style->border.clr, style->border.ol_clr);

	// add our label
	p_renderer->add_outlined_text_with_bg(top_left + label_pos, size, label, style->text.clr, style->text.ol_clr, style->text.bg_clr, style->text.size, style->text.ol_thckns);

	// add our buffered text
	p_renderer->add_outlined_text_with_bg(top_left + buffer_offset, size, buffer, style->buf_text.clr, style->buf_text.ol_clr, style->buf_text.bg_clr, style->buf_text.size, style->buf_text.ol_thckns);
}

widget_type text_entry::get_type()
{
	return widget_type::text_entry;
}

//
// combo box definitions
//

combo_box::combo_box(const vec2& top_left, const vec2& size, const std::wstring& label, combo_box_style* style) :
	widget(top_left, size, label, {20.f, 0.f}),
	style(style)
{ }

combo_box::combo_box(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos, combo_box_style* style) :
	widget(top_left, size, label, label_pos),
	style(style)
{ }

void combo_box::draw()
{
	p_renderer->add_outlined_frame(top_left, size, style->border.thckns, style->border.ol_thckns, style->border.clr, style->border.ol_clr);
	p_renderer->add_outlined_text_with_bg({ top_left.x + label_pos.x, top_left.y - style->text.size / 2.f - style->text.ol_thckns + label_pos.y }, size, label, style->text.clr, style->text.ol_clr, style->text.bg_clr, style->text.size);
}

widget_type combo_box::get_type()
{
	return widget_type::combo_box;
}

//
// color picker definitions
//

color_picker::color_picker(const vec2& top_left, const vec2& size, const std::wstring& label, color* p_color, color_picker_style* style) :
	widget(top_left, size, label),
	p_color(p_color),
	style(style),
	active_slider_index(-1),
	rgba_slider_size()
{
	instance_count++;
	calc_pos_and_sizes();
}

color_picker::color_picker(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos, color* p_color, color_picker_style* style) :
	widget(top_left, size, label, label_pos),
	p_color(p_color),
	style(style),
	active_slider_index(-1),
	rgba_slider_size()
{
	instance_count++;
	calc_pos_and_sizes();
}

void color_picker::calc_pos_and_sizes()
{
	// calculate header size and position (this is dependant on border thicknesses and text size)
	border_padding = 2.f * (style->border.thckns + style->border.ol_thckns);
	header_size = vec2{ size.x - border_padding * 2.f, p_renderer->measure_text(label, style->text.size).y };

	// calculate and store rgba slider positions and size (all relative to the top_left)
	rgba_slider_size = vec2{ header_size.x,  (size.y - header_size.y - border_padding * 3.f - style->sldr_gap * 3.f) / 4 };
	float slider_y_start_pos = header_size.y + border_padding * 2.f;

	for (auto i = 0u; i < 4; ++i)
		rgba_slider_pos[i] = vec2{ border_padding, slider_y_start_pos + i * (style->sldr_gap + rgba_slider_size.y) };
}

// check if a relative click is in any 4 of the rgba sliders, if so return the index of which one, else return -1
int color_picker::slider_click_index(const vec2& relative_mouse_pos) const
{
	for (auto i = 0; i < 4; ++i)
	{
		if (relative_mouse_pos.y >= rgba_slider_pos[i].y
			&& relative_mouse_pos.y <= rgba_slider_pos[i].y + rgba_slider_size.y
			&& relative_mouse_pos.x >= rgba_slider_pos[i].x
			&& relative_mouse_pos.x <= rgba_slider_pos[i].x + rgba_slider_size.x)
			return i;
	}

	return -1;
}

void color_picker::on_lbutton_down(const vec2& mouse_position)
{
	mouse_info.clicking = true;
	mouse_info.clicked = false;
	mouse_info.hovering = false;
	mouse_info.click_origin = relative_position(mouse_position);

	if (active)
	{
		auto relative = mouse_position - top_left;
		active_slider_index = slider_click_index(relative);
		if (active_slider_index != -1)
		{
			float ratio = (relative.x - rgba_slider_pos[active_slider_index].x) / rgba_slider_size.x;
			(*p_color)[active_slider_index] = ratio;

		}
	}
}

void color_picker::on_lbutton_up(const vec2& mouse_position)
{
	active_slider_index = -1;
	mouse_info.clicked = true;
	mouse_info.clicking = false;
	mouse_info.hovering = true;
}

void color_picker::on_drag(const vec2& new_position)
{
	if (active)
	{
		// check if relative click is inside any slider
		auto relative = new_position - top_left;

		if (active_slider_index == -1)
			active_slider_index = slider_click_index(relative);

		if (active_slider_index != -1)
		{
			float ratio = (relative.x - rgba_slider_pos[active_slider_index].x) / rgba_slider_size.x;
			(*p_color)[active_slider_index] = std::clamp<float>(ratio, 0.f, 1.f);

		}
	}
}

void color_picker::draw()
{
	static auto old_style = *style;
	static int updated = 0;
	if (old_style != *style)
	{
		calc_pos_and_sizes();
		if (++updated == instance_count)
		{
			old_style = *style;
			updated = 0;
		}
	}

	// add background
	p_renderer->add_rect_filled_multicolor(top_left, size, style->bg.tl_clr, style->bg.tr_clr, style->bg.bl_clr, style->bg.br_clr);

	// add frame
	p_renderer->add_outlined_frame(top_left, size, style->border.thckns, style->border.ol_thckns, style->border.clr, style->border.ol_clr);

	// draw label
	p_renderer->add_outlined_text_with_bg(top_left + border_padding, header_size, label, style->text.clr, style->text.ol_clr, style->text.bg_clr, style->text.size, style->text.ol_thckns);

	// draw preview square (aligned to the right of the header) on top of white square, preview should be a square, same size as the text y size
	vec2 preview_size{ header_size.y / 2.f, header_size.y / 2.f };
	vec2 preview_pos{ top_left.x + size.x - border_padding - preview_size.x * 2.f, top_left.y + border_padding };

	p_renderer->add_rect_filled(preview_pos, preview_size, { 1.f, 1.f, 1.f, 1.f });
	p_renderer->add_rect_filled({ preview_pos.x + preview_size.x, preview_pos.y }, preview_size, { .75f, .75f, .75f, .75f });
	p_renderer->add_rect_filled({ preview_pos.x, preview_pos.y + preview_size.y }, preview_size, { .75f, .75f, .75f, .75f });
	p_renderer->add_rect_filled(preview_pos + preview_size, preview_size, { 1.f, 1.f, 1.f, 1.f });

	p_renderer->add_rect_filled(preview_pos, preview_size * 2.f, *p_color);

	// draw sliders, these should take up the rest of 
	static color rgba[3] =
	{
		{1.f, 0.f, 0.f, 1.f},
		{0.f, 1.f, 0.f, 1.f},
		{0.f, 0.f, 1.f, 1.f}
	};

	for (auto i = 0u; i < 4; ++i)
	{
		p_renderer->add_rect_filled(top_left + rgba_slider_pos[i], { rgba_slider_size.x * (*p_color)[i], rgba_slider_size.y }, (i < 3 ? rgba[i] : color{ .5f, .5f, .5f, (*p_color)[i] * .7f + .3f }));
		p_renderer->add_outlined_frame(top_left + rgba_slider_pos[i], rgba_slider_size, style->sldr_border.thckns, style->sldr_border.ol_thckns, style->sldr_border.clr, style->sldr_border.ol_clr);
	}
}

widget_type color_picker::get_type()
{
	return widget_type::color_picker;
}