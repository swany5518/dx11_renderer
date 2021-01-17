#pragma once

#include <vector>
#include <queue>
#include <mutex>

#include "widgets.h"

//
// widget list class
// this is intended to act as an "internal window", it has a location, size and a list of widgets that belong to it, and input MSG queues 
// simple UI's can be made with 1 list the size of the window, but complex UI's should use multiple, especially if a lot of conditional renderering occurs
//

// a structure for finalized widget lists, this will be used to construct the widgets and bind them to style in the owned_styles list
struct owned_widget
{
	widget wdgt;			   // the actual widget object
	uint32_t owned_styles_idx; // the index the widget will have in the onwed styles list

	owned_widget() = delete;
	owned_widget(const widget& wdgt, uint32_t owned_styles_idx);
};

class widget_list
{
public:
	widget_list();
	widget_list(const vec2& top_left, const vec2& size);
	widget_list(const vec2& top_left, const vec2& size, const mc_rect& background);
	widget_list(const vec2& top_left, const vec2& size, const mc_rect& background, const std::vector<owned_widget>& owned_widgets, const std::vector<style>& owned_styles);

	// add a single widget to the widget list
	void add_widget(widget* p_widget);

	// add multiple widgets
	void add_widgets(widget* p_widgets, size_t count);

	// remove a widget by its address
	bool remove_widget(widget* p_widget);

	// remove a widget by its index in the list
	bool remove_widget(uint32_t idx);

	// check if a location is inside of a widget list
	bool contains(const vec2& pos);

	// insert an input message to the message queue
	void add_input_msg(const widget_input& msg);

	// activate/deactivate the widget list, deactivated lists will ignore added input messages and will not draw any widgets
	void set_active(bool active);

	// activate/deactive the move mode of the list, lists in move mode will allow widgets to be moved by dragging, and widgets will not respond to input how they normally would
	void set_move_mode(bool mode);

	// toggle move mode based on its currents state
	void toggle_move_mode();

	// get the current movement mode
	bool get_move_mode();

	// handles input messages from the queue and submits widget geometry to the gpu
	void draw_widgets();

	// print out needed code for the widget_list
	std::string to_string();

private:
	vec2 top_left;					      // widget list top left corner, all widgets will be relative to this
	vec2 size;						      // widget list size, usually will be the size of the window for simple UI's
	mc_rect background;					  // can contain a multicolored background that gets drawn under all widgets
	bool active;						  // if a widget list is active, the widgets will be drawn and inputs will be pushed into the queue, if not, it is "invisible"
	bool move_mode;						  // if move mode is true, widgets in the list can be dragged around for repositioning

	std::vector<widget*> widgets;				// vector of widget ptrs the list contains
	std::vector<owned_widget> owned_widgets;   // vector of widgets this instance owns
	std::vector<style> owned_styles;				// vector of styles this instance owns
	std::queue<widget_input> input_msgs;		// input message queue

	// handle next input message in the queue, if no messages are present, the function just returns, called in draw_widgets() before drawing
	void handle_next_input();
};

// retrieve a list needed to draw editing widgets for a given slider_style ptr
inline static widget_list get_slider_style_edit_list(slider_style* style)
{
	// create static widgets once to construct widget_list with, then replace ptr's with passed slider_style*

	// text style editing widgets
	static slider<float> text_size{		 {0,0},   {150,20},   L"text_size", 6.f, 50.f, &default_slider_style };
	static slider<float> text_ol_thckns{ {0,22},  {150,20},   L"text ol thckns", 0.f, 5.f, &default_slider_style };
	static color_picker text_clr{		 {0,44},  {150, 80},  L"text color",			&default_color_picker_style };
	static color_picker text_ol_clr{	 {0,126}, {150, 80},  L"text ol color",	&default_color_picker_style };
	static color_picker text_bg_clr{	 {0,208}, {150, 80},  L"text bg color", &default_color_picker_style };

	// border style editing widgets
	static slider<float> border_thckns{		{0,290}, {150,20}, L"border thckns", 0.f, 5.f, &default_slider_style };
	static slider<float> border_ol_thckns{	{0,312}, {150,20}, L"border ol thickness", 0.f, 5.f, &default_slider_style };
	static color_picker border_clr{			{0,334}, {150, 80}, L"border color", &default_color_picker_style };
	static color_picker border_ol_clr{		{0,416}, {150, 80}, L"border ol color", &default_color_picker_style };

	// background style editing widgets
	static color_picker bg_tl{ {0,498},	{150, 80}, L"bg top left",		&default_color_picker_style };
	static color_picker bg_tr{ {0,580},	{150, 80}, L"bg top right",		&default_color_picker_style };
	static color_picker bg_bl{ {0,662},	{150, 80}, L"bg btm left",	&default_color_picker_style };
	static color_picker bg_br{ {0,744},	{150, 80}, L"bg btm right",	&default_color_picker_style };

	// slide style editing widgets
	static color_picker slide_tl{ {0,826},	{150, 80}, L"slide top left",		&default_color_picker_style };
	static color_picker slide_tr{ {0,908},	{150, 80}, L"slide top right",		&default_color_picker_style };
	static color_picker slide_bl{ {0,990},	{150, 80}, L"slide btm left",	&default_color_picker_style };
	static color_picker slide_br{ {0,1072},	{150, 80}, L"slide btm right",	&default_color_picker_style };

	text_size.set_value_ptr(&style->text.size);
	text_ol_thckns.set_value_ptr(&style->text.ol_thckns);
	text_clr.set_color_ptr(&style->text.clr);
	text_ol_clr.set_color_ptr(&style->text.ol_clr);
	text_bg_clr.set_color_ptr(&style->text.bg_clr);

	border_thckns.set_value_ptr(&style->border.thckns);
	border_ol_thckns.set_value_ptr(&style->border.ol_thckns);
	border_clr.set_color_ptr(&style->border.clr);
	border_ol_clr.set_color_ptr(&style->border.ol_clr);

	bg_tl.set_color_ptr(&style->bg.tl_clr);
	bg_tr.set_color_ptr(&style->bg.tr_clr);
	bg_bl.set_color_ptr(&style->bg.bl_clr);
	bg_br.set_color_ptr(&style->bg.br_clr);

	slide_tl.set_color_ptr(&style->clr.tl_clr);
	slide_tr.set_color_ptr(&style->clr.tr_clr);
	slide_bl.set_color_ptr(&style->clr.bl_clr);
	slide_br.set_color_ptr(&style->clr.br_clr);

	widget_list list{ vec2{0,0}, vec2{1000,1000}, color{0.f, 0.f, 0.f, 1.f} };
	list.add_widget(&text_size);
	list.add_widget(&text_ol_thckns);
	list.add_widget(&text_clr);
	list.add_widget(&text_ol_clr);
	list.add_widget(&text_bg_clr);
	list.add_widget(&border_thckns);
	list.add_widget(&border_ol_thckns);
	list.add_widget(&border_clr);
	list.add_widget(&border_ol_clr);
	list.add_widget(&bg_tl);
	list.add_widget(&bg_tr);
	list.add_widget(&bg_bl);
	list.add_widget(&bg_br);
	list.add_widget(&slide_tl);
	list.add_widget(&slide_tr);
	list.add_widget(&slide_bl);
	list.add_widget(&slide_br);

	return list;
}