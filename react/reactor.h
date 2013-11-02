
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

	virtual bool mousePress( const draw::point &p, int button ) = 0;
	virtual bool mouseRelease( const draw::point &p, int button ) = 0;
	virtual bool mouseMove( const draw::point &p ) = 0;
};

////////////////////////////////////////

}

