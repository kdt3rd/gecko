
#pragma once

#include "background.h"

namespace gui
{

////////////////////////////////////////

class background_color : public background
{
public:
	background_color( const base::color &c = { 0.13, 0.13, 0.13, 1 }, const std::shared_ptr<widget> &w = std::shared_ptr<widget>() );
	~background_color( void );

	void set_color( const base::color &c ) { _color = c; }

	void paint( const std::shared_ptr<draw::canvas> &c ) override;

private:
	base::color _color = { 0.13, 0.13, 0.13, 1 };
};

////////////////////////////////////////

}

