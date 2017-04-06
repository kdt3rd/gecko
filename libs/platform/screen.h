//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <base/size.h>
#include <base/point.h>

namespace platform
{

////////////////////////////////////////

/// @brief Screen device.
///
/// A screen device
class screen
{
public:
	/// @brief Constructor.
	screen( void );

	/// @brief Destructor.
	virtual ~screen( void );

	virtual bool is_default( void ) const = 0;

	/// @brief Screen bounds.
	///
	/// The bounds (size) of the screen.
	/// @return The size of the screen
	virtual base::size bounds( void ) const = 0;

	/// @brief Screen DPI
	///
	/// Returns the horizontal and vertical DPI for the screen
	/// @return The size of the screen
	virtual base::size dpi( void ) const = 0;
};

////////////////////////////////////////

}

