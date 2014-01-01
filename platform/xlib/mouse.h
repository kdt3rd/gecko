
#pragma once

#include <platform/mouse.h>

namespace xlib
{

////////////////////////////////////////

/// @brief Xlib implementation of platform::mouse.
class mouse : public platform::mouse
{
public:
	/// @brief Constructor.
	mouse( void );

	/// @brief Destructor.
	virtual ~mouse( void );
};

////////////////////////////////////////

}

