// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include "window.h"

#include <base/contract.h>
#include <base/pointer.h>
#include <iostream>
#include <stdexcept>

namespace platform
{
namespace dummy
{
////////////////////////////////////////

window::window( void ) {}

////////////////////////////////////////

window::~window( void ) {}

////////////////////////////////////////

void window::raise( void ) {}

////////////////////////////////////////

void window::lower( void ) {}

////////////////////////////////////////

void window::show( void ) {}

////////////////////////////////////////

void window::hide( void ) {}

////////////////////////////////////////

bool window::is_visible( void )
{
    // TODO fix this
    return true;
}

////////////////////////////////////////

/*
rect window::geometry( void )
{
}
*/

////////////////////////////////////////

void window::resize( double w, double h ) {}

////////////////////////////////////////

void window::set_minimum_size( double w, double h ) {}

////////////////////////////////////////

void window::set_title( const std::string &t ) {}

////////////////////////////////////////

void window::invalidate( const draw::rect & ) {}

////////////////////////////////////////

} // namespace dummy
} // namespace platform
