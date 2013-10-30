
#pragma once

#include <draw/area.h>
#include <draw/canvas.h>
#include <draw/rect.h>

namespace view
{

////////////////////////////////////////

class view
{
public:
	view( const std::shared_ptr<draw::area> &a );
	virtual ~view( void );

	std::shared_ptr<draw::area> area( void );
	bool contains( double x, double y ) { return _area->contains( x, y ); }

	virtual void paint( const std::shared_ptr<draw::canvas> &canvas ) = 0;

protected:
	std::shared_ptr<draw::area> _area;
};

////////////////////////////////////////

}

