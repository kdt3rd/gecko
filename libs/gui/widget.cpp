//
// Copyright (c) 2015 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "widget.h"
#include <base/contract.h>

////////////////////////////////////////

namespace gui
{

////////////////////////////////////////

widget::widget( void )
{
}

////////////////////////////////////////

widget::~widget( void )
{
}

////////////////////////////////////////

void widget::build( gl::api &ogl )
{
}

////////////////////////////////////////

void widget::paint( gl::api &ogl )
{
}

////////////////////////////////////////

bool widget::mouse_press( const base::point &p, int button )
{
	unused( p );
	unused( button );
	return false;
}

////////////////////////////////////////

bool widget::mouse_release( const base::point &p, int button )
{
	unused( p );
   	unused( button );
	return false;
}

////////////////////////////////////////

bool widget::mouse_move( const base::point &p )
{
	unused( p );
	return false;
}

////////////////////////////////////////

bool widget::mouse_wheel( int a )
{
	unused( a );
	return false;
}

////////////////////////////////////////

bool widget::key_press( platform::scancode c )
{
	unused( c );
	return false;
}

////////////////////////////////////////

bool widget::key_release( platform::scancode c )
{
	unused( c );
	return false;
}

////////////////////////////////////////

bool widget::text_input( char32_t c )
{
	unused( c );
	return false;
}

////////////////////////////////////////

}



