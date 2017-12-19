//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "background.h"

namespace gui
{

////////////////////////////////////////

background::background( const std::shared_ptr<widget> &w )
	: _widget( w )
{
}

////////////////////////////////////////

background::~background( void )
{
}

////////////////////////////////////////

void background::paint( const std::shared_ptr<draw::canvas> &canvas )
{
	if ( _widget )
		_widget->paint( canvas );
}

////////////////////////////////////////

bool background::mouse_press( const point &p, int button )
{
	if ( _widget )
		return _widget->mouse_press( p, button );
	return widget::mouse_press( p, button );
}

////////////////////////////////////////

bool background::mouse_release( const point &p, int button )
{
	if ( _widget )
		return _widget->mouse_release( p, button );
	return widget::mouse_release( p, button );
}

////////////////////////////////////////

bool background::mouse_move( const point &p )
{
	if ( _widget )
		return _widget->mouse_move( p );
	return widget::mouse_move( p );
}

////////////////////////////////////////

void background::compute_minimum( void )
{
	if ( _widget )
	{
		_widget->compute_minimum();
		set_minimum( _widget->minimum() );
	}
}

////////////////////////////////////////

void background::compute_layout( void )
{
	if ( _widget )
	{
		_widget->set( position(), extent() );
		_widget->compute_layout();
	}
}

////////////////////////////////////////

}

