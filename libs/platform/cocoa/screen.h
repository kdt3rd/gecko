
#pragma once

#include <platform/screen.h>

namespace platform { namespace cocoa
{

////////////////////////////////////////

/// @brief Cocoa implementation of screen.
class screen : public platform::screen
{
public:
	screen( void );
	~screen( void );

	base::size bounds( void ) const override;

private:
};

////////////////////////////////////////

} }

