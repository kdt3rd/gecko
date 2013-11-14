
#include <iostream>
#include "button.h"

namespace reaction
{

////////////////////////////////////////

button::button( void )
{
}

////////////////////////////////////////

button::~button( void )
{
}

////////////////////////////////////////

bool button::mouse_press( const draw::point &p, int button )
{
	if ( contains( p ) )
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
	if ( contains( p ) )
		activated();
	return true;
}

////////////////////////////////////////

bool button::mouse_move( const draw::point &p )
{
	pressed( contains( p ) );
	return true;
}

////////////////////////////////////////

}

