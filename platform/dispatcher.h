
#pragma once

namespace platform
{

////////////////////////////////////////

class dispatcher
{
public:
	dispatcher( void );
	virtual ~dispatcher( void );

	virtual int execute( void ) = 0;
	virtual void exit( int code ) = 0;
};

////////////////////////////////////////

}

