
#pragma once

#include <platform/screen.h>

namespace platform { namespace cocoa
{

class system;

////////////////////////////////////////

/// @brief Cocoa implementation of screen.
class screen : public platform::screen
{
public:
	screen( void ) = delete;
	screen( void *scr );
	~screen( void );

	bool is_default( void ) const override;

	base::size bounds( void ) const override;
	base::size dpi( void ) const override;

private:
	friend class platform::cocoa::system;

	void *_nsscreen;
};

////////////////////////////////////////

} }

