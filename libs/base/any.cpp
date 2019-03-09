// Copyright (c) 2014-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include "any.h"

namespace base
{

////////////////////////////////////////

bad_any_cast::~bad_any_cast( void )
{
}

////////////////////////////////////////

const char *bad_any_cast::what() const noexcept
{
	return "bad any cast";
}

} // base namespace
