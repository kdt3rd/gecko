
#pragma once

#include "widget.h"
#include "delegate.h"

namespace platform
{
	class window;
}

namespace gui
{

////////////////////////////////////////

class window : public delegate
{
public:
	window( const std::shared_ptr<platform::window> &w );
	~window( void );

	void set_title( const std::string &t );

	void show( void );

	void set_widget( const std::shared_ptr<widget> &w );
	std::shared_ptr<widget> get_widget( void ) { return _widget; }

	void invalidate( const draw::rect &r );

private:
	void paint( void );
	void resize( double w, double h );

	void mouse_press( const draw::point &p, int button );
	void mouse_release( const draw::point &p, int button );

	std::shared_ptr<layout::simple_area> _area;
	std::shared_ptr<platform::window> _window;
	std::shared_ptr<widget> _widget;
};

////////////////////////////////////////

}

