
#pragma once

#include <functional>

namespace platform
{

////////////////////////////////////////

class mouse
{
public:
	mouse( void );
	virtual ~mouse( void );

	void when_pressed( std::function<void(int)> f ) { _pressed = f; }
	void when_released( std::function<void(int)> f ) { _released = f; }
	void when_moved( std::function<void(double,double)> f ) { _moved = f; }

	void moved( double x, double y ) { if ( _moved ) _moved( x, y ); }
	void pressed( int button ) { if ( _pressed ) _pressed( button ); }
	void released( int button ) { if ( _released ) _released( button ); }

private:
	std::function<void(double,double)> _moved;
	std::function<void(int)> _pressed;
	std::function<void(int)> _released;
};

////////////////////////////////////////

}

