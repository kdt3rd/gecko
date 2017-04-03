
#pragma once

#include <platform/keyboard.h>

namespace platform { namespace cocoa
{

////////////////////////////////////////

/// @brief Cocoa implementation of keyboard.
class keyboard : public ::platform::keyboard
{
public:
	keyboard( void );
	~keyboard( void );

	void update_mapping( void );

	void start( void ) override;
	void cancel( void ) override;
	void shutdown( void ) override;
	intptr_t poll_object( void ) override;
	bool poll_timeout( duration &when, const time_point &curtime ) override;
	void emit( const time_point &curtime ) override;

//	xcb_keysym_t get_keysym( xcb_keycode_t code, uint16_t state );
	::platform::scancode get_scancode( unsigned short code );

private:
};

////////////////////////////////////////

} }

