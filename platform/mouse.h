
#pragma once

#include <memory>
#include <functional>
#include "window.h"

namespace platform
{

////////////////////////////////////////

class mouse
{
public:
	mouse( void );
	virtual ~mouse( void );

	void when_pressed( std::function<void(const std::shared_ptr<window>&,int)> f ) { _pressed = f; }
	void when_released( std::function<void(const std::shared_ptr<window>&,int)> f ) { _released = f; }
	void when_moved( std::function<void(const std::shared_ptr<window>&,double,double)> f ) { _moved = f; }

	void moved( const std::shared_ptr<window> &w, double x, double y ) { if ( _moved ) _moved( w, x, y ); }
	void pressed( const std::shared_ptr<window> &w, int button ) { if ( _pressed ) _pressed( w, button ); }
	void released( const std::shared_ptr<window> &w, int button ) { if ( _released ) _released( w, button ); }

private:
	std::function<void(const std::shared_ptr<window>&,double,double)> _moved;
	std::function<void(const std::shared_ptr<window>&,int)> _pressed;
	std::function<void(const std::shared_ptr<window>&,int)> _released;
};

////////////////////////////////////////

}

