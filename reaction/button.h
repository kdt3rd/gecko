
#pragma once

#include <memory>
#include "reaction.h"
#include <core/action.h>
#include <layout/area.h>

namespace reaction
{

////////////////////////////////////////

class button : public reaction
{
public:
	button( void );
	~button( void );

	action<void( bool )> pressed;
	action<void( void )> activated;

	virtual bool mouse_press( const draw::point &p, int button );
	virtual bool mouse_release( const draw::point &p, int button );
	virtual bool mouse_move( const draw::point &p );
};

////////////////////////////////////////

}

