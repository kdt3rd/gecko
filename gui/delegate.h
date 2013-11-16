
#pragma once

#include <draw/rect.h>

namespace gui
{

////////////////////////////////////////

class delegate
{
public:
	virtual ~delegate( void ) {}

	virtual void invalidate( const draw::rect &r ) = 0;
};

////////////////////////////////////////

}

