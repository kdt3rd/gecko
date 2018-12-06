//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once

#include <platform/mouse.h>

namespace platform { namespace dummy
{

////////////////////////////////////////

class mouse : public platform::mouse
{
public:
	mouse( void );
	~mouse( void );
};

////////////////////////////////////////

} }

