
#include "platform.h"
#include <platform/mswin/system.h>
//#include <platform/dummy/system.h>

namespace platform
{

////////////////////////////////////////

void platform::init( void )
{
	platform::platform::enroll( "mswin", "gl", [] { return std::make_shared<mswin::system>(); } );
//	platform::platform::enroll( "dummy", "dummy", [] { return std::make_shared<dummy::system>(); } );
}

}
