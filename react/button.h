
#pragma once

#include <memory>
#include "reactor.h"
#include <draw/area.h>

namespace react
{

////////////////////////////////////////

class button : public reactor
{
public:
	button( const std::shared_ptr<draw::area> &a );
	~button( void );

	virtual bool mouse_press( const draw::point &p, int button );
	virtual bool mouse_release( const draw::point &p, int button );
	virtual bool mouse_move( const draw::point &p );

private:
	std::shared_ptr<draw::area> _area;
};

////////////////////////////////////////

}

