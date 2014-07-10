
#pragma once

#include "widget.h"

namespace gui
{

////////////////////////////////////////

class color_picker : public widget
{
public:
	color_picker( void );

	void paint( const std::shared_ptr<draw::canvas> &c ) override;

	bool mouse_press( const base::point &p, int b ) override;
	bool mouse_release( const base::point &p, int b ) override;
	bool mouse_move( const base::point &p ) override;

private:
	bool _tracking = false;
	base::color _current;
};

////////////////////////////////////////

}

