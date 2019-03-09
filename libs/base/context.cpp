// Copyright (c) 2018 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "context.h"

////////////////////////////////////////

namespace base
{

////////////////////////////////////////

context::context( void )
{
}

////////////////////////////////////////

context::~context( void )
{
}

////////////////////////////////////////

void context::share( const context &c )
{
    _stash = c._stash;
}

////////////////////////////////////////

context::stash_type &
context::stash( void )
{
    if ( ! _stash )
        _stash = std::make_shared<stash_type>();
    return (*_stash);
}

////////////////////////////////////////

} // base
