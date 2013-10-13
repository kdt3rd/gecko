
#pragma once

#include <core/size.h>
#include <platform/screen.h>

namespace sdl
{

////////////////////////////////////////

class screen : public platform::screen
{
public:
	screen( int idx );
	virtual ~screen( void );

	virtual size bounds( void );

private:
	int _index;
};

////////////////////////////////////////

}

