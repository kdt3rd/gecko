
#pragma once

#include <platform/mouse.h>

namespace xcb
{

////////////////////////////////////////

class mouse : public platform::mouse
{
public:
	mouse( void );
	virtual ~mouse( void );
};

////////////////////////////////////////

}

