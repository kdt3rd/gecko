
#pragma once

#include <platform/keyboard.h>

namespace xcb
{

////////////////////////////////////////

class keyboard : public platform::keyboard
{
public:
	keyboard( void );
	virtual ~keyboard( void );
};

////////////////////////////////////////

}

