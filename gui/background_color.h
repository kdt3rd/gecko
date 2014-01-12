
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
	background_color( datum<core::color> &&c = { 1, 1, 1, 1 }, const std::shared_ptr<widget> &w = std::shared_ptr<widget>() );
	~background_color( void );

	void set_color( const core::color &c ) { _color = c; }

	void paint( const std::shared_ptr<gldraw::canvas> &c ) override;

private:
	datum<core::color> _color = { 1, 0, 1, 1 };
};

////////////////////////////////////////

}

