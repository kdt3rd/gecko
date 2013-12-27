
#pragma once

#include "widget.h"
#include <core/alignment.h>
#include <core/signal.h>

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

	virtual void compute_minimum( void );

	virtual bool mouse_press( const draw::point &p, int button );
	virtual bool mouse_release( const draw::point &p, int button );
	virtual bool mouse_move( const draw::point &p );

	core::signal<void(void)> when_activated;

private:
	bool _pressed = false;
	bool _tracking = false;
	std::string _text;
	alignment _align = alignment::CENTER;
	draw::color _color = { 0, 0, 0, 1 };
	std::shared_ptr<draw::font> _font;
};

////////////////////////////////////////

}

