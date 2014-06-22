
#pragma once

#include <platform/keyboard.h>

namespace platform { namespace dummy
{

////////////////////////////////////////

class keyboard : public platform::keyboard
{
public:
	keyboard( void );
	~keyboard( void );
};

////////////////////////////////////////

} }

