
#pragma once

#include <functional>

namespace platform
{

////////////////////////////////////////

class timer
{
public:
	timer( void );
	virtual ~timer( void );

	virtual void schedule( double seconds ) = 0;
	virtual void cancel( void ) = 0;

	void when_elapsed( const std::function<void(void)> &f ) { _elapsed = f; }

	void elapsed( void ) { if ( _elapsed ) _elapsed(); }

private:
	std::function<void(void)> _elapsed;
};

////////////////////////////////////////

}

