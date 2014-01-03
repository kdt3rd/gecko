
#pragma once

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
	~screen( void );

	draw::size bounds( void ) override;

private:
};

////////////////////////////////////////

}

