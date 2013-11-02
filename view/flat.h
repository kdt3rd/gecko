
#pragma once

#include "view.h"

namespace view
{

////////////////////////////////////////

class flat : public view
{
public:
	flat( const std::shared_ptr<draw::area> &a );
	virtual ~flat( void );

	void set_color( const draw::color &c ) { _color = c; }

	virtual void paint( const std::shared_ptr<draw::canvas> &canvas );

private:
	std::shared_ptr<draw::area> _area;
	draw::color _color;
};

////////////////////////////////////////

}

