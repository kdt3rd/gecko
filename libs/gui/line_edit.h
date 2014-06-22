
#pragma once

#include "widget.h"
#include "application.h"
#include "style.h"
#include <model/datum.h>
#include <base/alignment.h>

namespace gui
{

////////////////////////////////////////

class line_edit : public widget
{
public:
	line_edit( void );
	line_edit( datum<std::string> &&l, datum<alignment> &&a = alignment::LEFT, datum<base::color> &&c = { 0, 0, 0, 1 }, shared_datum<draw::font> &&f = application::current_style()->default_font() );
	~line_edit( void );

	const std::string &text( void ) { return _text.value(); }
	void set_text( const std::string &t ) { _text = t; }

	void set_font( std::shared_ptr<draw::font> &f ) { _font = f; }
	void set_align( alignment a ) { _align = a; }

	void paint( const std::shared_ptr<draw::canvas> &c ) override;

	void compute_minimum( void ) override;

	bool key_press( platform::scancode c ) override;

	bool text_input( char32_t c ) override;

private:
	size_t _cursor = 0;
	datum<std::string> _text;
	datum<alignment> _align = alignment::LEFT;
	datum<base::color> _color = { 0, 0, 0, 1 };
	shared_datum<draw::font> _font = application::current_style()->default_font();
};

////////////////////////////////////////

}

