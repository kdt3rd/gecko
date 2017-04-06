//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <platform/mouse.h>

namespace platform { namespace xcb
{

////////////////////////////////////////

/// @brief XCB implementation of platform::mouse.
class mouse : public platform::mouse
{
public:
	/// @brief Constructor.
	mouse( void );

	/// @brief Destructor.
	~mouse( void );
};

////////////////////////////////////////

} }

