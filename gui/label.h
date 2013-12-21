
#pragma once

#include "widget.h"
#include <core/alignment.h>

namespace gui
{

////////////////////////////////////////

class label : public widget
{
public:
	label( void );
	label( const std::string &l, alignment a = alignment::LEFT );
	~label( void );

	const std::string &text( void ) { return _text; }
	void set_text( const std::string &t ) { _text = t; }

	void set_font( std::shared_ptr<draw::font> &f ) { _font = f; }
	void set_align( alignment a ) { _align = a; }

	virtual void paint( const std::shared_ptr<draw::canvas> &c );

	virtual void compute_minimum( void );

private:
	std::string _text;
	alignment _align = alignment::LEFT;
	draw::color _color = { 0, 0, 0, 1 };
	std::shared_ptr<draw::font> _font;
};

////////////////////////////////////////

}

