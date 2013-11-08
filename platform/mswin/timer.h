
#pragma once

#include <platform/timer.h>

namespace mswin
{

////////////////////////////////////////

class timer : public platform::timer
{
public:
	timer( void );
	virtual ~timer( void );

	virtual void schedule( double seconds );
	virtual void cancel( void );

private:
	double _timeout = 0.0;
};

////////////////////////////////////////

}

