
#include "platform.h"
#include <platform/xlib/system.h>
//#include <platform/xcb/system.h>
//#include <platform/dummy/system.h>

namespace platform
{

////////////////////////////////////////

void platform::init( void )
{
	platform::platform::enroll( "xlib", "cairo", [] { return std::make_shared<xlib::system>(); } );
//	platform::platform::enroll( "xcb", "cairo", [] { return std::make_shared<xcb::system>(); } );
//	platform::platform::enroll( "dummy", "dummy", [] { return std::make_shared<dummy::system>(); } );
}

}
