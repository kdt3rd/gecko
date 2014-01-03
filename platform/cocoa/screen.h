
#pragma once

#include <platform/screen.h>

namespace cocoa
{

////////////////////////////////////////

/// @brief Cocoa implementation of screen.
class screen : public platform::screen
{
public:
	screen( void );
	~screen( void );

	draw::size bounds( void ) override;

private:
};

////////////////////////////////////////

}

