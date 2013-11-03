
#pragma once

#include <core/size.h>
#include <platform/screen.h>

namespace dummy
{

////////////////////////////////////////

class screen : public platform::screen
{
public:
	screen( void );
	virtual ~screen( void );

	virtual size bounds( void );
};

////////////////////////////////////////

}

