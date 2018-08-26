//
// Copyright (c) 2018 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

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



