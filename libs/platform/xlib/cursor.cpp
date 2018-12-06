//
// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT
// Copyrights licenced under the MIT License.
//

#include "cursor.h"

////////////////////////////////////////
namespace platform { namespace xlib
{

////////////////////////////////////////

cursor::cursor( Cursor c )
	: _handle( c )
{
}

////////////////////////////////////////

cursor::~cursor( void )
{
}

////////////////////////////////////////

bool
cursor::supports_color( void ) const
{
	// fill in w/ queries against Xcursor
	return true;
}

////////////////////////////////////////

bool
cursor::supports_animation( void ) const
{
	// fill in w/ queries against Xcursor
	return true;
}

////////////////////////////////////////

bool
cursor::is_animated( void ) const
{
	// TODO: this is handled by the server, if the create cursor call is made
	// correctly...
	return false;
}

////////////////////////////////////////

} // namespace xlib
} // namespace platform



