
#pragma once

#include "reaction.h"

#include <memory>
#include <core/action.h>

namespace reaction
{

////////////////////////////////////////

class passive : public reaction
{
public:
	passive( void );
	~passive( void );

	virtual bool mouse_press( const layout::simple_area &area, const draw::point &p, int button );
	virtual bool mouse_release( const layout::simple_area &area, const draw::point &p, int button );
	virtual bool mouse_move( const layout::simple_area &area, const draw::point &p );
};

////////////////////////////////////////

}

