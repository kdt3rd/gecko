
#pragma once

#include <layout/simple_area.h>
#include <draw/point.h>

namespace reaction
{

////////////////////////////////////////

class reaction
{
public:
	reaction( void );
	virtual ~reaction( void );

	virtual bool mouse_press( const layout::simple_area &a, const draw::point &p, int button ) = 0;
	virtual bool mouse_release( const layout::simple_area &a, const draw::point &p, int button ) = 0;
	virtual bool mouse_move( const layout::simple_area &a, const draw::point &p ) = 0;
};

////////////////////////////////////////

}

