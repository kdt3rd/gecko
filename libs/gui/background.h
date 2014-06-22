
#pragma once

#include "widget.h"

namespace gui
{

////////////////////////////////////////

class background : public widget
{
public:
	background( const std::shared_ptr<widget> &w = std::shared_ptr<widget>() );
	~background( void );

	void set_widget( const std::shared_ptr<widget> &v )
	{
		_widget = v;
	}

	void paint( const std::shared_ptr<draw::canvas> &canvas ) override;

	bool mouse_press( const base::point &p, int button ) override;
	bool mouse_release( const base::point &p, int button ) override;
	bool mouse_move( const base::point &p ) override;

	void compute_minimum( void ) override;
	void compute_layout( void ) override;

private:
	std::shared_ptr<widget> _widget;
};

////////////////////////////////////////

}

