//
// Copyright (c) 2018 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//


#include "cursor.h"

////////////////////////////////////////

namespace platform
{
namespace cocoa
{

////////////////////////////////////////

cursor::cursor( id c )
	: _handle( c )
{
	if ( _handle )
		[_handle retain];
}

////////////////////////////////////////

cursor::~cursor( void )
{
	if ( _handle )
		[_handle release];
}

////////////////////////////////////////

bool cursor::supports_color( void ) const
{
	return true;
}

////////////////////////////////////////

bool cursor::supports_animation( void ) const
{
	return true;
}

////////////////////////////////////////

bool cursor::is_animated( void ) const
{
	return false;
}

////////////////////////////////////////

} // namespace cocoa
} // namespace platform

