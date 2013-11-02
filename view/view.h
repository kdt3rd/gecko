
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
	view( void );
	virtual ~view( void );

	virtual void paint( const std::shared_ptr<draw::canvas> &canvas ) = 0;
};

////////////////////////////////////////

}

