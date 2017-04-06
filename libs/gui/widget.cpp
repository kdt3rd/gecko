//
// Copyright (c) 2015 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "widget.h"


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


void widget::paint( const std::shared_ptr<draw::canvas> & /*canvas*/ )
{
}


////////////////////////////////////////


bool widget::mouse_press( const base::point & /*p*/, int /*button*/ )
{
	return false;
}


////////////////////////////////////////


bool widget::mouse_release( const base::point & /*p*/, int /*button*/ )
{
	return false;
}


////////////////////////////////////////


bool widget::mouse_move( const base::point & /*p*/ )
{
	return false;
}


////////////////////////////////////////


bool widget::mouse_wheel( int /*a*/ )
{
	return false;
}


////////////////////////////////////////


bool widget::key_press( platform::scancode /*c*/ )
{
	return false;
}


////////////////////////////////////////


bool widget::key_release( platform::scancode /*c*/ )
{
	return false;
}


////////////////////////////////////////


bool widget::text_input( char32_t /*c*/ )
{
	return false;
}


////////////////////////////////////////


} // gui



