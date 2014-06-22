
#include "platform.h"
#include <platform/cocoa/system.h>
//#include <platform/dummy/system.h>
#include <clocale>

namespace platform
{

////////////////////////////////////////

void platform::init( void )
{
	std::setlocale( LC_ALL, "" );

	platform::platform::enroll( "cocoa", "gl", [] { return std::make_shared<cocoa::system>(); } );
//	platform::platform::enroll( "dummy", "dummy", [] { return std::make_shared<dummy::system>(); } );
}

}
