
#pragma once

#include "widget.h"
#include "application.h"
#include "style.h"
#include <model/datum.h>
#include <base/alignment.h>

namespace gui
{

////////////////////////////////////////

class label : public widget
{
public:
	label( void );
	label( datum<std::string> &&l, datum<alignment> &&a = alignment::LEFT, datum<base::color> &&c = { 0, 0, 0, -1 }, shared_datum<script::font> &&f = application::current_style()->default_font() );
	~label( void );

	const std::string &text( void ) { return _text.value(); }
	void set_text( const std::string &t ) { _text = t; }

	void set_font( std::shared_ptr<script::font> &f ) { _font = f; }
	void set_align( alignment a ) { _align = a; }

	void paint( const std::shared_ptr<draw::canvas> &c ) override;

	void compute_minimum( void ) override;

private:
	datum<std::string> _text;
	datum<alignment> _align = alignment::LEFT;
	datum<base::color> _color = { 0, 0, 0, 1 };
	shared_datum<script::font> _font = application::current_style()->default_font();;
};

////////////////////////////////////////

}

