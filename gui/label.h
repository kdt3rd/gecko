
#pragma once

#include "widget.h"
#include "application.h"
#include "style.h"
#include <model/datum.h>
#include <core/alignment.h>

namespace gui
{

////////////////////////////////////////

class label : public widget
{
public:
	label( void );
	label( datum<std::string> &&l, datum<alignment> &&a = alignment::LEFT, datum<draw::color> &&c = { 0, 0, 0, 1 }, shared_datum<draw::font> &&f = application::current_style()->default_font() );
	~label( void );

	const std::string &text( void ) { return _text.value(); }
	void set_text( const std::string &t ) { _text = t; }

	void set_font( std::shared_ptr<draw::font> &f ) { _font = f; }
	void set_align( alignment a ) { _align = a; }

	virtual void paint( const std::shared_ptr<draw::canvas> &c );

	virtual void compute_minimum( void );

private:
	datum<std::string> _text;
	datum<alignment> _align = alignment::LEFT;
	datum<draw::color> _color = { 0, 0, 0, 1 };
	shared_datum<draw::font> _font = application::current_style()->default_font();;
};

////////////////////////////////////////

}

