//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <base/size.h>
#include <base/rect.h>

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
	///
	/// TODO: add methods to query color management (i.e. ICM under
	/// windows), change the resolution / display rate on the fly,
	/// etc.
	screen( void );

	/// @brief Destructor.
	virtual ~screen( void );

	/// @brief default state
	///
	/// @return Bool indicating whether this screen should be
	/// considered the 'default' screen
	virtual bool is_default( void ) const = 0;

	virtual double refresh_rate( void ) const = 0;

	/// @brief Screen bounds.
	///
	/// The bounds (size) of the screen, either the full (avail ==
	/// false) or the available user space (avail == true)
	/// 
	/// @return The size of the screen
	virtual base::rect bounds( bool avail ) const = 0;

	/// @brief Screen DPI
	///
	/// Returns the horizontal and vertical DPI for the screen
	/// @return The size of the screen
	virtual base::size dpi( void ) const = 0;
};

////////////////////////////////////////

}

