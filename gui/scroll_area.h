
#pragma once

#include "widget.h"

namespace gui
{

////////////////////////////////////////

class scroll_area : public widget
{
public:
	scroll_area( void );
	scroll_area( double minw, double minh );
	virtual ~scroll_area( void );

	virtual void set_delegate( delegate *d );

	void set_widget( const std::shared_ptr<widget> &v )
	{
		_widget = v;
	}

	virtual void paint( const std::shared_ptr<draw::canvas> &canvas );

	virtual bool mouse_press( const draw::point &p, int button );
	virtual bool mouse_release( const draw::point &p, int button );
	virtual bool mouse_move( const draw::point &p );

	virtual void compute_layout( void );

private:
	draw::point _position;
	std::shared_ptr<widget> _widget;
};

////////////////////////////////////////

}

