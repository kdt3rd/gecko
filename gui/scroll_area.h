
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
	~scroll_area( void );

	void set_widget( const std::shared_ptr<widget> &v )
	{
		_widget = v;
	}

	void paint( const std::shared_ptr<draw::canvas> &canvas ) override;

	bool mouse_press( const draw::point &p, int button ) override;
	bool mouse_release( const draw::point &p, int button ) override;
	bool mouse_move( const draw::point &p ) override;

	void compute_layout( void ) override;

private:
	draw::point _position;
	std::shared_ptr<widget> _widget;
};

////////////////////////////////////////

}

