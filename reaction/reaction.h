
#pragma once

#include <layout/area.h>
#include <draw/point.h>

namespace reaction
{

////////////////////////////////////////

class reaction : public layout::area
{
public:
	reaction( void );
	virtual ~reaction( void );

	virtual bool mouse_press( const draw::point &p, int button ) = 0;
	virtual bool mouse_release( const draw::point &p, int button ) = 0;
	virtual bool mouse_move( const draw::point &p ) = 0;
};

////////////////////////////////////////

}

