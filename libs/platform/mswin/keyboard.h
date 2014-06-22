
#pragma once

#undef KEY_EXECUTE
#include <platform/keyboard.h>

namespace mswin
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

