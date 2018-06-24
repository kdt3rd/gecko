//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "waitable.h"

////////////////////////////////////////

namespace platform
{

////////////////////////////////////////

waitable::waitable( system *s )
    : event_source( s )
{
}

////////////////////////////////////////

waitable::~waitable( void )
{
}

////////////////////////////////////////

} // platform



