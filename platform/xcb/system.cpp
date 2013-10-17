
#include "system.h"
#include "screen.h"
#include "window.h"
#include "timer.h"
#include "dispatcher.h"

#include <stdexcept>

namespace xcb
{

////////////////////////////////////////

system::system( void )
	: platform::system( "xcb", "XCB" )
{
	int prefScreen = 0;
	_connection = xcb_connect( nullptr, &prefScreen );

	const xcb_setup_t *setup = xcb_get_setup( _connection );
	xcb_screen_iterator_t iter = xcb_setup_roots_iterator( setup );
	_screen = iter.data;

	while ( iter.rem > 0 )
	{
		_screens.push_back( std::make_shared<screen>( iter.data ) );
		xcb_screen_next( &iter );
	}

	_dispatcher = std::make_shared<dispatcher>();
}

////////////////////////////////////////

system::~system( void )
{
	xcb_disconnect( _connection );
}

////////////////////////////////////////

std::shared_ptr<platform::window> system::new_window( void )
{
	auto ret = std::make_shared<window>( _connection, _screen );
	_dispatcher->add_window( ret );
	return ret;
}

////////////////////////////////////////

std::shared_ptr<platform::timer> system::new_timer( void )
{
	return std::make_shared<timer>();
}

////////////////////////////////////////

std::shared_ptr<platform::dispatcher> system::dispatch( void )
{
	return _dispatcher;
}


////////////////////////////////////////

}
