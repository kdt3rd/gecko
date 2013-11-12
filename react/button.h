
#pragma once

#include <memory>
#include "reactor.h"
#include <core/action.h>
#include <layout/area.h>

namespace react
{

////////////////////////////////////////

class button : public reactor
{
public:
	button( const std::shared_ptr<layout::area> &a );
	~button( void );

	action<void( bool )> pressed;
	action<void( void )> activated;

	virtual bool mouse_press( const draw::point &p, int button );
	virtual bool mouse_release( const draw::point &p, int button );
	virtual bool mouse_move( const draw::point &p );

private:
	std::shared_ptr<layout::area> _area;
};

////////////////////////////////////////

}

