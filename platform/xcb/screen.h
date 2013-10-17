
#pragma once

#include <core/size.h>
#include <platform/screen.h>
#include <xcb/xcb.h>

namespace xcb
{

////////////////////////////////////////

class screen : public platform::screen
{
public:
	screen( xcb_screen_t *scr );
	virtual ~screen( void );

	virtual size bounds( void );

	xcb_screen_t *id( void ) { return _screen; }

private:
	xcb_screen_t *_screen;
};

////////////////////////////////////////

}

