
#pragma once

#include <core/size.h>
#include <platform/screen.h>

namespace x11
{

////////////////////////////////////////

/// @brief X11 implementation of platform::screen
class screen : public platform::screen
{
public:
	/// @brief Constructor.
	screen( void );
	virtual ~screen( void );

	virtual size bounds( void );

private:
};

////////////////////////////////////////

}

