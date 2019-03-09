// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "cursor.h"
#include "system.h"
#include <base/contract.h>

////////////////////////////////////////

namespace platform
{

////////////////////////////////////////

cursor::cursor( void )
{
}

////////////////////////////////////////

cursor::~cursor( void )
{
}

////////////////////////////////////////

std::shared_ptr<cursor>
cursor::load( standard_cursor sc, const std::shared_ptr<system> &s )
{
	precondition( s, "invalid system" );
	return s->builtin_cursor( sc );
}

} // platform



