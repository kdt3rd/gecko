
#pragma once

#include "widget.h"
#include "application.h"
#include "style.h"
#include <core/alignment.h>
#include <core/signal.h>
#include <model/datum.h>

namespace gui
{

////////////////////////////////////////

class button : public widget
{
public:
	button( void );
	button( datum<std::string> &&l, datum<alignment> &&a = alignment::LEFT, datum<core::color> &&c = { 0, 0, 0, 1 }, shared_datum<draw::font> &&f = application::current_style()->default_font() );
	~button( void );

	const std::string &text( void )
	{
		return _text.value();
	}

	void set_text( const std::string &t )
	{
		_text = t;
	}

	void set_font( std::shared_ptr<draw::font> &f )
	{
		_font = f;
	}

	void set_pressed( bool p );

	void paint( const std::shared_ptr<draw::canvas> &c ) override;

	void compute_minimum( void ) override;

	bool mouse_press( const core::point &p, int button ) override;
	bool mouse_release( const core::point &p, int button ) override;
	bool mouse_move( const core::point &p ) override;

	core::signal<void(void)> when_activated;

private:
	datum<std::string> _text;
	datum<alignment> _align = alignment::CENTER;
	datum<core::color> _color = { 0, 0, 0, 1 };
	shared_datum<draw::font> _font = application::current_style()->default_font();

	bool _pressed = false;
	bool _tracking = false;
};

////////////////////////////////////////

}

