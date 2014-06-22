
#pragma once

#include <platform/mouse.h>

namespace platform { namespace dummy
{

////////////////////////////////////////

class mouse : public platform::mouse
{
public:
	mouse( void );
	~mouse( void );
};

////////////////////////////////////////

} }

