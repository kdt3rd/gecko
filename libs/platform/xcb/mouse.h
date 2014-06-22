
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

