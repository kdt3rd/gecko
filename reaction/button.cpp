
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

bool button::mouse_press( const layout::simple_area &area, const draw::point &p, int button )
{
	if ( area.contains( p ) )
	{
		_tracking = true;
		pressed( true );
		return true;
	}
	return false;
}

////////////////////////////////////////

bool button::mouse_release( const layout::simple_area &area, const draw::point &p, int button )
{
	if ( _tracking )
	{
		pressed( false );
		if ( area.contains( p ) )
			activated();
		return true;
	}
	return false;
}

////////////////////////////////////////

bool button::mouse_move( const layout::simple_area &area, const draw::point &p )
{
	if ( _tracking )
	{
		pressed( area.contains( p ) );
		return true;
	}
	return false;
}

////////////////////////////////////////

}

