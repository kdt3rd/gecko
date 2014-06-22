
#include "platform.h"
#include <platform/xlib/system.h>
//#include <platform/xcb/system.h>
//#include <platform/dummy/system.h>
#include <clocale>

namespace platform
{

////////////////////////////////////////

void platform::init( void )
{
	std::setlocale( LC_ALL, "" );

	platform::platform::enroll( "xlib", "gl", [] { return std::make_shared<xlib::system>(); } );
//	platform::platform::enroll( "xcb", "cairo", [] { return std::make_shared<xcb::system>(); } );
//	platform::platform::enroll( "dummy", "dummy", [] { return std::make_shared<dummy::system>(); } );
}

}
