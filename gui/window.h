
#pragma once

#include "widget.h"
#include "context.h"
#include <platform/keyboard.h>

namespace platform
{
	class window;
}

namespace gui
{

////////////////////////////////////////

class window : public context
{
public:
	window( const std::shared_ptr<platform::window> &w );
	~window( void );

	void set_title( const std::string &t );

	void show( void );

	void set_widget( const std::shared_ptr<widget> &w );
	std::shared_ptr<widget> get_widget( void ) { return _widget; }

	void invalidate( const draw::rect &r ) override;

private:
	void paint( void );
	void resize( double w, double h );

	void mouse_press( const draw::point &p, int button );
	void mouse_release( const draw::point &p, int button );
	void mouse_moved( const draw::point &p );

	void key_pressed( platform::scancode c );
	void key_released( platform::scancode c );
	void text_entered( char32_t c );

	std::shared_ptr<layout::simple_area> _area;
	std::shared_ptr<platform::window> _window;
	std::shared_ptr<widget> _widget;
};

////////////////////////////////////////

}

