//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "composite.h"
#include "label.h"
#include "radio_button.h"

namespace gui
{

////////////////////////////////////////

void composite::build( context &ctxt )
{
	for_subwidgets( [&]( const std::shared_ptr<widget> &w )
	{
		w->build( ctxt );
	} );
}

////////////////////////////////////////

void composite::paint( context &ctxt )
{
	for_subwidgets( [&]( const std::shared_ptr<widget> &w )
	{
		w->paint( ctxt );
	} );
}

////////////////////////////////////////

bool composite::mouse_press( const point &p, int button )
{
	if ( _mouse_grab )
		return _mouse_grab->mouse_press( p, button );
	else
	{
		for_subwidgets( [&]( const std::shared_ptr<widget> &w )
		{
			if ( !_mouse_grab )
			{
				if ( w->mouse_press( p, button ) )
					_mouse_grab = w;
			}
		} );
	}

	return static_cast<bool>( _mouse_grab );
}

////////////////////////////////////////

bool composite::mouse_move( const point &p )
{
	bool result = false;
	if ( _mouse_grab )
		result = _mouse_grab->mouse_move( p );
	else
	{
		for_subwidgets( [&]( const std::shared_ptr<widget> &w )
		{
			if ( !result )
				result = w->mouse_move( p );
		} );
	}
	return result;
}

////////////////////////////////////////

bool composite::mouse_release( const point &p, int button )
{
	bool result = false;
	if ( _mouse_grab )
	{
		auto tmp = _mouse_grab;
		_mouse_grab.reset();
		result = tmp->mouse_release( p, button );
	}
	else
	{
		for_subwidgets( [&]( const std::shared_ptr<widget> &w )
		{
			if ( !result )
				result = w->mouse_release( p, button );
		} );
	}

	return result;
}

////////////////////////////////////////

bool composite::mouse_wheel( int amount )
{
	bool result = false;
	for_subwidgets( [&]( const std::shared_ptr<widget> &w )
	{
		if ( !result )
			result = w->mouse_wheel( amount );
	} );

	return result;
}

////////////////////////////////////////

bool composite::key_press( platform::scancode c )
{
	bool result = false;
	for_subwidgets( [&]( const std::shared_ptr<widget> &w )
	{
		if ( !result )
			result = w->key_press( c );
	} );

	return result;
}

////////////////////////////////////////

bool composite::key_release( platform::scancode c )
{
	bool result = false;
	for_subwidgets( [&]( const std::shared_ptr<widget> &w )
	{
		if ( !result )
			result = w->key_release( c );
	} );

	return result;
}

////////////////////////////////////////

bool composite::text_input( char32_t c )
{
	bool result = false;
	for_subwidgets( [&]( const std::shared_ptr<widget> &w )
	{
		if ( !result )
			result = w->text_input( c );
	} );

	return result;
}

////////////////////////////////////////

bool composite::update_layout( double duration )
{
	bool result = widget::update_layout( duration );
	for_subwidgets( [&]( const std::shared_ptr<widget> &w )
	{
		result = w->update_layout( duration ) | result;
	} );
	return result;
}

////////////////////////////////////////

}

