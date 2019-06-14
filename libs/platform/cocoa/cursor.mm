// Copyright (c) 2018 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "cursor.h"

////////////////////////////////////////

namespace platform
{
namespace cocoa
{
////////////////////////////////////////

cursor::cursor( id c ) : _handle( c )
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

bool cursor::supports_color( void ) const { return true; }

////////////////////////////////////////

bool cursor::supports_animation( void ) const { return true; }

////////////////////////////////////////

bool cursor::is_animated( void ) const { return false; }

////////////////////////////////////////

} // namespace cocoa
} // namespace platform
