
#pragma once

#include <core/size.h>

namespace platform
{

////////////////////////////////////////

class screen
{
public:
	screen( void );
	virtual ~screen( void );

	virtual size bounds( void ) = 0;
};

////////////////////////////////////////

}

