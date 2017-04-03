
#pragma once

#include <platform/mouse.h>

namespace platform { namespace cocoa
{

////////////////////////////////////////

/// @brief Cocoa implementation of mouse.
class mouse : public ::platform::mouse
{
public:
	mouse( void );
	~mouse( void );

	void start( void ) override;
	void cancel( void ) override;
	void shutdown( void ) override;
	intptr_t poll_object( void ) override;
	bool poll_timeout( duration &when, const time_point &curtime ) override;
	void emit( const time_point &curtime ) override;

};

////////////////////////////////////////

} }

