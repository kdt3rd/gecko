
#pragma once

#include "widget.h"
#include "application.h"
#include <base/alignment.h>

namespace gui
{

////////////////////////////////////////

class label : public widget
{
public:
	label( void );
	label( std::string l, base::alignment a = base::alignment::LEFT, const base::color &c = { 1.0, 1.0, 1.0, 1.0 }, const std::shared_ptr<script::font> &f = application::current()->get_default_font() );
	~label( void );

	const std::string &text( void ) const { return _text; }
	void set_text( std::string t ) { _text.swap( t ); }

	const base::color &color( void ) const { return _color; }
	void set_color( const base::color &c ) { _color = c; }

	const std::shared_ptr<script::font> &font( void ) const { return _font; }
	void set_font( const std::shared_ptr<script::font> &f ) { _font = f; }

	base::alignment alignment( void ) const { return _align; }
	void set_align( base::alignment a ) { _align = a; }

	void paint( const std::shared_ptr<draw::canvas> &c ) override;

	void compute_minimum( void ) override;

private:
	std::string _text;
	base::alignment _align = base::alignment::LEFT;
	base::color _color = { 1.0, 1.0, 1.0, 1.0 };
	std::shared_ptr<script::font> _font = application::current()->get_default_font();
};

////////////////////////////////////////

}

