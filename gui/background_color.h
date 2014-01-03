
#pragma once

#include "background.h"
#include <model/datum.h>
#include <core/alignment.h>

namespace gui
{

////////////////////////////////////////

class background_color : public background
{
public:
	background_color( datum<draw::color> &&c = { 1, 1, 1, 1 }, const std::shared_ptr<widget> &w = std::shared_ptr<widget>() );
	~background_color( void );

	void set_color( const draw::color &c ) { _color = c; }

	void paint( const std::shared_ptr<draw::canvas> &c ) override;

private:
	datum<draw::color> _color = { 1, 0, 1, 1 };
};

////////////////////////////////////////

}

