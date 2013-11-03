
#pragma once

#include <platform/mouse.h>

namespace dummy
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

