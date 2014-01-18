
#include "platform.h"
#include <platform/cocoa/system.h>
//#include <platform/dummy/system.h>

namespace platform
{

////////////////////////////////////////

void platform::init( void )
{
	platform::platform::enroll( "cocoa", "cairo", [] { return std::make_shared<cocoa::system>(); } );
//	platform::platform::enroll( "dummy", "dummy", [] { return std::make_shared<dummy::system>(); } );
}

}
