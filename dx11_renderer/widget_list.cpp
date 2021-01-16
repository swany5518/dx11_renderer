#include "widget_list.h"

// widget list definitions

widget_list::widget_list() :
	top_left({0.f, 0.f}),
	size({5000.f, 5000.f}),
	background(),
	active(true),
	move_mode(false),
	widgets(),
	input_msgs()
{ }

widget_list::widget_list(const vec2& top_left, const vec2& size) :
	top_left(top_left),
	size(size),
	background(),
	active(true),
	move_mode(false),
	widgets(),
	input_msgs()
{ }

widget_list::widget_list(const vec2& top_left, const vec2& size, const mc_rect& background) :
	top_left(top_left),
	size(size),
	background(background),
	active(true),
	move_mode(false),
	widgets(),
	input_msgs()
{ }

void widget_list::add_widget(widget* p_widget)
{
	widgets.push_back(p_widget);
}

void widget_list::add_widgets(widget* p_widgets, size_t count)
{
	for (auto i = 0u; i < count; ++i)
		widgets.push_back(&p_widgets[i]);
}

bool widget_list::remove_widget(widget* p_widget)
{
	auto position = std::find(widgets.begin(), widgets.end(), p_widget);

	if (position == widgets.end())
		return false;
	
	widgets.erase(position);

	return true;
}

bool widget_list::remove_widget(uint32_t idx)
{
	if (idx < 0 || idx >= widgets.size())
		return false;

	widgets.erase(widgets.begin() + idx);

	return true;
}

void widget_list::add_input_msg(const widget_input& msg)
{
	if (!active)
		return;

	input_msgs.push(msg);
}

bool widget_list::contains(const vec2& pos)
{
	return pos.x >= top_left.x
		&& pos.x <= top_left.x + size.x
		&& pos.y >= top_left.y
		&& pos.y <= top_left.y + size.y;
}

void widget_list::draw_widgets()
{
	if (!active)
		return;

	handle_next_input();

	for (auto widget : widgets)
		widget->draw();
}

void widget_list::handle_next_input()
{
	if (input_msgs.empty())
		return;
	
	auto& msg = input_msgs.front();

	if (msg.type == input_type::mouse_move)
	{
		if (move_mode)
		{
			for (auto widget : widgets)
				if (widget->mouse_info.clicking)
					widget->on_widget_move(msg.m_pos);
		}
		else
		{
			for (auto widget : widgets)
				if (widget->mouse_info.clicking)
					widget->on_drag(msg.m_pos);
		}
	}
	else if (msg.type == input_type::lbutton_down)
	{
		for (auto widget : widgets)
			if (widget->contains(msg.m_pos))
				widget->on_lbutton_down(msg.m_pos);
	}
	else if (msg.type == input_type::lbutton_up)
	{
		for (auto widget : widgets)
		{
			if (widget->contains(msg.m_pos))
				widget->on_lbutton_up(msg.m_pos);
			else
				widget->mouse_info.clicking = false;
		}
	}
	else if (msg.type == input_type::key_press)
	{
		for (auto widget : widgets)
			widget->on_key_down(msg.key);
	}
	
	input_msgs.pop();
}

void widget_list::set_active(bool active)
{
	this->active = active;
}

void widget_list::set_move_mode(bool mode)
{
	move_mode = mode;
}

std::string widget_list::to_string()
{
	return "";
}

//inline static widget_list