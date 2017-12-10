//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "widget.h"
#include <draw/rectangle.h>
#include <base/signal.h>

namespace gui
{

////////////////////////////////////////

class scroll_bar : public widget
{
public:
	using scroll_value = coord_type;

	scroll_bar( bool bounded = true );
	~scroll_bar( void );

	scroll_value value( void )
	{
		return _value;
	}

	void set_horizontal( void )
	{
		_horizontal = true;
	}

	void set_vertical( void )
	{
		_horizontal = false;
	}

	void set_value( scroll_value v );
	void set_handle( scroll_value h );
	void set_page( scroll_value p );
	void set_range( scroll_value min, scroll_value max );

	void build( gl::api &ogl ) override;
	void paint( gl::api &ogl ) override;

	bool mouse_press( const point &p, int button ) override;
	bool mouse_move( const point &p ) override;
	bool mouse_release( const point &p, int button ) override;

	signal<void(scroll_value)> when_changing;

private:
	void update_value( scroll_value v );

	scroll_value translate_to_full_w( scroll_value v )
	{
		return _min + ( v - x1() ) * ( _max - _min ) / width();
	}

	scroll_value translate_from_full_w( scroll_value v )
	{
		return x1() + ( v - _min ) * width() / ( _max - _min );
	}

	scroll_value translate_to_full_h( scroll_value v )
	{
		return _min + ( v - y1() ) * ( _max - _min ) / height();
	}

	scroll_value translate_from_full_h( scroll_value v )
	{
		return y1() + ( v - _min ) * height() / ( _max - _min );
	}

	scroll_value _start = 0.0;

	scroll_value _value = 0.0;
	scroll_value _handle = 20.0;
	scroll_value _page = 0.0;
	scroll_value _min = 0.0, _max = 100.0;

	bool _tracking = false;
	bool _horizontal = true;
	bool _bounded = true;

	draw::rectangle _groove;
	draw::rectangle _knob;
};

////////////////////////////////////////

}

