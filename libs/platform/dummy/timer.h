
#pragma once

#include <platform/timer.h>

namespace dummy
{

////////////////////////////////////////

class timer : public platform::timer
{
public:
	timer( void );
	~timer( void );

	void schedule( double seconds ) override;
	void cancel( void ) override;

private:
	double _timeout = 0.0;
};

////////////////////////////////////////

}

