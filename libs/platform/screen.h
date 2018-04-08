//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <memory>
#include "types.h"

namespace color
{
class standard_definition;
}

namespace platform
{

class renderer;

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

	/// @brief determine if screen has a window manager running
	///
	/// if this returns false, the screen should be used as a full
	/// screen-only screen.
	virtual bool is_managed( void ) const = 0;

	/// @brief Screen bounds.
	///
	/// The bounds (size) of the screen, either the full (avail ==
	/// false) or the available user space (avail == true)
	/// 
	/// @return The size of the screen
	virtual rect bounds( bool avail ) const = 0;

	/// @brief Screen DPI
	///
	/// Returns the horizontal and vertical DPI for the screen
	/// @return The size of the screen
	virtual base::dsize dpi( void ) const = 0;

	/// @brief returns the current refresh rate of the screen
	virtual double refresh_rate( void ) const = 0;

	/// @brief Retrieve the renderer to be used for this screen
	virtual const std::shared_ptr<renderer> &render( void ) const = 0;

	/// @brief Screen display standard
	///
	/// The output color space. This should be used to transform all
	/// UI colors from their specified space to the output space
	/// appropriate for this screen.
	virtual const color::standard_definition &display_standard( void ) const = 0;

	/// @brief Force display standard to be used
	///
	/// This can be used to force a particular display standard be
	/// used for the screen
	virtual void override_display_standard( const color::standard_definition & ) = 0;
};

////////////////////////////////////////

}

