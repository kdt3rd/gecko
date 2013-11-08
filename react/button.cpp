
#include <iostream>
#include "button.h"

namespace react
{

////////////////////////////////////////

button::button( const std::shared_ptr<draw::area> &a )
	: _area( a )
{
}

////////////////////////////////////////

button::~button( void )
{
}

////////////////////////////////////////

bool button::mouse_press( const draw::point &p, int button )
{
	if ( _area->contains( p ) )
	{
		if ( _pressed )
			_pressed( true );
		return true;
	}
	return false;
}

////////////////////////////////////////

bool button::mouse_release( const draw::point &p, int button )
{
	_pressed( false );
	return false;
}

////////////////////////////////////////

bool button::mouse_move( const draw::point &p )
{
	return false;
}

////////////////////////////////////////

}
