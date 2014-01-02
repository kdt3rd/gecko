
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
	~screen( void );

	size bounds( void ) override;
};

////////////////////////////////////////

}

