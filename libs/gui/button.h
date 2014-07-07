
#pragma once

#include "widget.h"
#include "application.h"
#include <draw/stretchable.h>
#include <base/alignment.h>
#include <base/signal.h>

namespace gui
{

////////////////////////////////////////

class button : public widget
{
public:
	button( void );
	button( std::string l, base::alignment a = base::alignment::CENTER, const base::color &c = { 1.0, 1.0, 1.0 }, const std::shared_ptr<script::font> &f = application::current()->get_default_font() );
	~button( void );

	const std::string &text( void ) const
	{
		return _text;
	}

	void set_text( const std::string &t )
	{
		_text = t;
	}

	void set_font( const std::shared_ptr<script::font> &f )
	{
		_font = f;
	}

	void set_pressed( bool p );

	void paint( const std::shared_ptr<draw::canvas> &c ) override;

	void compute_minimum( void ) override;

	bool mouse_press( const base::point &p, int button ) override;
	bool mouse_release( const base::point &p, int button ) override;
	bool mouse_move( const base::point &p ) override;

	base::signal<void(void)> when_activated;

private:
	std::string _text;
	base::alignment _align = base::alignment::CENTER;
	base::color _color = { 1.0, 1.0, 1.0 };
	std::shared_ptr<script::font> _font = application::current()->get_default_font();

	std::shared_ptr<draw::stretchable> _draw;

	bool _pressed = false;
	bool _tracking = false;
};

////////////////////////////////////////

}

