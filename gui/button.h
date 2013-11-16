
#pragma once

#include "widget.h"
#include <core/alignment.h>

namespace gui
{

////////////////////////////////////////

class button : public widget
{
public:
	button( void );
	button( const std::string &l );
	~button( void );

	const std::string &text( void ) { return _text; }
	void set_text( const std::string &t ) { _text = t; }

	void set_font( std::shared_ptr<draw::font> &f ) { _font = f; }

	void set_pressed( bool p );

	virtual void paint( const std::shared_ptr<draw::canvas> &c );

	virtual void layout( void );

private:
	bool _pressed = false;
	std::string _text;
	alignment _align = alignment::CENTER;
	draw::color _color = { 0, 0, 0, 1 };
	std::shared_ptr<draw::font> _font;
};

////////////////////////////////////////

}

