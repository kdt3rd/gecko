//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <functional>
#include "waitable.h"
#include "scancode.h"

namespace platform
{

////////////////////////////////////////

/// @brief Keyboard device.
///
/// A keyboard device that can send key press/release events.
class keyboard : public waitable
{
public:
	/// @brief Constructor.
	keyboard( void );

	/// @brief Destructor.
	virtual ~keyboard( void );

private:
};

////////////////////////////////////////

}

