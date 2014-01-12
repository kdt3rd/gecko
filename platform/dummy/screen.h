
#pragma once

#include <platform/screen.h>

namespace dummy
{

////////////////////////////////////////

class screen : public platform::screen
{
public:
	screen( void );
	~screen( void );

	core::size bounds( void ) override;
};

////////////////////////////////////////

}

