//
// Copyright (c) 2015 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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



