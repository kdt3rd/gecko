
#pragma once

#include <core/size.h>

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

	/// @brief Screen bounds.
	///
	/// The bounds (size) of the screen.
	/// @return The size of the screen
	virtual size bounds( void ) = 0;
};

////////////////////////////////////////

}

