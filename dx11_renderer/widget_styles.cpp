#pragma once

#include "widget_styles.h"

std::string text_style::to_string() const
{
	return "text_style{\n" +
		std::to_string(size) + ",\n" +
		std::to_string(ol_thckns) + ",\n" +
		clr.to_string()	+ ",\n" +
		ol_clr.to_string() + ",\n" +
		bg_clr.to_string() + " }";
}

std::string border_style::to_string() const
{
	return "border_style{\n" +
		std::to_string(thckns) + ",\n" +
		std::to_string(ol_thckns) + ",\n" +
		clr.to_string()	+ ",\n" +
		ol_clr.to_string() + " }";
}

std::string mc_rect::to_string() const
{
	return "mc_rect{\n" +
		tl_clr.to_string() + ",\n" +
		tr_clr.to_string() + ",\n" +
		bl_clr.to_string() + ",\n" +
		br_clr.to_string() + " }";
}

std::string checkbox_style::to_string() const
{
	return "checkbox_style{ " + 
		text.to_string() + ",\n" +
		border.to_string() + ",\n" +
		bg.to_string() + ",\n" +
		check.to_string() + ",\n" +
		std::to_string(gap) + "\n}";
}

std::string button_style::to_string() const
{
	return "checkbox_style{ " +
		text.to_string() + ",\n" +
		border.to_string() + ",\n" +
		bg.to_string() + " }";
}

std::string slider_style::to_string() const
{
	return "slider_style{ " +
		text.to_string() + ",\n" +
		border.to_string() + ",\n" +
		bg.to_string() + " }";
}

std::string text_entry_style::to_string() const
{
	return "text_entry_style{ " + 
		text.to_string() + ",\n" +
		buf_text.to_string() + ",\n" +
		border.to_string() + ",\n" +
		bg.to_string() + " }";
}

std::string combo_box_style::to_string() const
{
	return "combo_box_style{ " +
		text.to_string() + ",\n" +
		border.to_string() + ",\n" +
		bg.to_string() + " }";
}

std::string color_picker_style::to_string() const
{
	return "color_picker_style{ " +
		text.to_string() + ",\n" +
		border.to_string() + ",\n" +
		bg.to_string() + ",\n" +
		sldr_border.to_string() + ",\n" +
		std::to_string(sldr_gap) + " }";
}

bool color_picker_style::operator==(const color_picker_style& other) const
{
	return !memcmp(this, &other, sizeof(color_picker_style));
}

bool color_picker_style::operator!=(const color_picker_style& other) const
{
	return memcmp(this, &other, sizeof(color_picker_style));
}