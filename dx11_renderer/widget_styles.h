#pragma once

#include <string>

#include "renderer_utils.h"

//
// generic styles most widgets will have
//

// widget text styling
struct text_style 
{
	float size;			// text font size
	float ol_thckns;	// text outline thickness
	color clr;			// text font color
	color ol_clr;		// text outline color
	color bg_clr;		// text background color

	// return string with styles required code
	std::string to_string() const; 
};

// widget border styling
struct border_style
{
	float thckns;		// border thickness
	float ol_thckns;	// border outline thickness
	color clr;			// border color
	color ol_clr;		// border outline color

	// print out the style's required code
	std::string to_string() const;
};

// multicolored rect, usually used for backgrounds or rects with gradients
struct mc_rect
{
	color tl_clr;	// top left color
	color tr_clr;	// top right color
	color bl_clr;	// bottom left color
	color br_clr;	// bottom right color

	// print out the style's required code
	std::string to_string() const;
};

//
// specific widget styles
//

struct checkbox_style
{
	text_style text;	 // checkbox text styling
	border_style border; // checkbox border styling
	mc_rect bg;			 // checkbox background styling
	mc_rect check;		 // checkbox check styling
	float gap;			 // checkbox check gap from border

	std::string to_string() const;
};

struct button_style
{
	text_style text;	 // button text styling
	border_style border; // button border styling
	mc_rect bg;			 // button background style

	std::string to_string() const;
};

struct slider_style
{
	text_style text;	 // slider text styling
	border_style border; // slider border styling
	mc_rect bg;			 // slider background styling
	mc_rect clr;		 // slider bar color

	std::string to_string() const;
};

struct text_entry_style
{
	text_style text;	 // text entry text style
	text_style buf_text; // text entry buffer text style
	border_style border; // text entry border styling
	mc_rect bg;			 // text entry background styling

	std::string to_string() const;
};

struct combo_box_style
{
	text_style text;	 // combo box text styling
	border_style border; // combo box border styling
	mc_rect bg;			 // combo box background styling

	std::string to_string() const;
};

struct color_picker_style
{
	text_style text;		  // color picker text styling
	border_style border;	  // color picker border styling
	mc_rect bg;				  // color picker background styling
	border_style sldr_border; // color picker slider border styling
	float sldr_gap;			  // color picker slider gap between sliders

	std::string to_string() const;

	bool operator==(const color_picker_style&) const;
	bool operator!=(const color_picker_style&) const;
};
