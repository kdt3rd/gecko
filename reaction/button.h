
#pragma once

#include <memory>
#include "reaction.h"
#include <core/action.h>

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

	virtual bool mouse_press( const layout::simple_area &area, const draw::point &p, int button );
	virtual bool mouse_release( const layout::simple_area &area, const draw::point &p, int button );
	virtual bool mouse_move( const layout::simple_area &area, const draw::point &p );
};

////////////////////////////////////////

}

