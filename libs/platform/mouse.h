//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once

#include "waitable.h"

namespace platform
{

////////////////////////////////////////

/// @brief Mouse device.
///
/// Mouse device which can send button press/release and mouse motion events.
class mouse : public waitable
{
public:

	/// @brief Constructor.
	mouse( system *s );

	/// @brief Destructor.
	virtual ~mouse( void );

private:
};

////////////////////////////////////////

}

