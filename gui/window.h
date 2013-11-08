
#pragma once

#include "container.h"

namespace platform
{
	class window;
}

namespace gui
{

////////////////////////////////////////

class window
{
public:
	window( const std::shared_ptr<platform::window> &w );
	~window( void );

	void set_title( const std::string &t );

	void show( void );

	std::shared_ptr<container> get_container( void ) { return _container; }

private:
	void paint( void );
	void resize( double w, double h );

	void mouse_press( const draw::point &p, int button );

	std::shared_ptr<draw::area> _area;
	std::shared_ptr<platform::window> _window;
	std::shared_ptr<container> _container;
};

////////////////////////////////////////

}

