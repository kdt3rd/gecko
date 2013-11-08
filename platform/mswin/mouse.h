
#pragma once

#include <platform/mouse.h>

namespace mswin
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

