
#pragma once

#include <draw/point.h>

namespace react
{

////////////////////////////////////////

class reactor
{
public:
	reactor( void );
	virtual ~reactor( void );

	virtual bool mouse_press( const draw::point &p, int button ) = 0;
	virtual bool mouse_release( const draw::point &p, int button ) = 0;
	virtual bool mouse_move( const draw::point &p ) = 0;
};

////////////////////////////////////////

}

