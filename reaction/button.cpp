
#include <iostream>
#include "button.h"

namespace reaction
{

////////////////////////////////////////

button::button( const std::shared_ptr<layout::area> &a )
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
		pressed( true );
		return true;
	}
	return false;
}

////////////////////////////////////////

bool button::mouse_release( const draw::point &p, int button )
{
	pressed( false );
	if ( _area->contains( p ) )
		activated();
	return true;
}

////////////////////////////////////////

bool button::mouse_move( const draw::point &p )
{
	pressed( _area->contains( p ) );
	return true;
}

////////////////////////////////////////

}
