
#include <iostream>
#include "passive.h"

namespace reaction
{

////////////////////////////////////////

passive::passive( void )
{
}

////////////////////////////////////////

passive::~passive( void )
{
}

////////////////////////////////////////

bool passive::mouse_press( const draw::point &p, int button )
{
	return false;
}

////////////////////////////////////////

bool passive::mouse_release( const draw::point &p, int button )
{
	return false;
}

////////////////////////////////////////

bool passive::mouse_move( const draw::point &p )
{
	return false;
}

////////////////////////////////////////

}

