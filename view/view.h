
#pragma once

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

	virtual void paint( const std::shared_ptr<draw::canvas> &canvas, const draw::rect &area ) = 0;
};

////////////////////////////////////////

}

