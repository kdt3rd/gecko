
#pragma once

#include "widget.h"
#include <core/alignment.h>

namespace gui
{

////////////////////////////////////////

class color : public widget
{
public:
	color( void );
	color( const draw::color &c, double minw = 0.0, double minh = 0.0 );
	~color( void );

	void set_color( const draw::color &c ) { _color = c; }

	virtual void paint( const std::shared_ptr<draw::canvas> &c );

private:
	draw::color _color = { 1, 0, 1, 1 };
};

////////////////////////////////////////

}

