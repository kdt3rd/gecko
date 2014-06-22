
#pragma once

#include <platform/mouse.h>

namespace platform { namespace cocoa
{

////////////////////////////////////////

/// @brief Cocoa implementation of mouse.
class mouse : public platform::mouse
{
public:
	mouse( void );
	~mouse( void );
};

////////////////////////////////////////

} }

