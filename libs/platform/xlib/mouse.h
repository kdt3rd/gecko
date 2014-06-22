
#pragma once

#include <platform/mouse.h>

namespace platform { namespace xlib
{

////////////////////////////////////////

/// @brief Xlib implementation of platform::mouse.
class mouse : public ::platform::mouse
{
public:
	/// @brief Constructor.
	mouse( void );

	/// @brief Destructor.
	~mouse( void );
};

////////////////////////////////////////

} }

