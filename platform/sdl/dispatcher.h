
#pragma once

#include <platform/dispatcher.h>

namespace sdl
{

////////////////////////////////////////

class dispatcher : public platform::dispatcher
{
public:
	dispatcher( void );
	virtual ~dispatcher( void );

	int execute( void );
	void exit( int code );
};

////////////////////////////////////////

}
