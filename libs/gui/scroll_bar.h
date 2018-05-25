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

class scroll_bar_w : public widget
{
public:
	using value_type = coord_type;

	scroll_bar_w( bool bounded = true );
	~scroll_bar_w( void );

	value_type value( void )
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

	void set_value( value_type v );
	void set_handle( value_type h );
	void set_page( value_type p );
	void set_range( value_type min, value_type max );

	void build( context &ctxt ) override;
	void paint( context &ctxt ) override;

	bool mouse_press( const event &e ) override;
	bool mouse_move( const event &e ) override;
	bool mouse_release( const event &e ) override;

	signal<void(value_type)> when_changing;

private:
	void update_value( value_type v );

	value_type translate_to_full_w( value_type v )
	{
		return _min + ( v - x1() ) * ( _max - _min ) / width();
	}

	value_type translate_from_full_w( value_type v )
	{
		return x1() + ( v - _min ) * width() / ( _max - _min );
	}

	value_type translate_to_full_h( value_type v )
	{
		return _min + ( v - y1() ) * ( _max - _min ) / height();
	}

	value_type translate_from_full_h( value_type v )
	{
		return y1() + ( v - _min ) * height() / ( _max - _min );
	}

	value_type _start = 0.0;

	value_type _value = 0.0;
	value_type _handle = 20.0;
	value_type _page = 0.0;
	value_type _min = 0.0, _max = 100.0;

	bool _tracking = false;
	bool _horizontal = true;
	bool _bounded = true;

	draw::rectangle _groove;
	draw::rectangle _knob;
};

////////////////////////////////////////

using scroll_bar = widget_ptr<scroll_bar_w>;

}

