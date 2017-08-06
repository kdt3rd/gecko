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
	scroll_bar( bool bounded = true );
	~scroll_bar( void );

	double value( void )
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

	void set_value( double v );
	void set_handle( double h );
	void set_page( double p );
	void set_range( double min, double max );

	void build( gl::api &ogl ) override;
	void paint( gl::api &ogl ) override;

	bool mouse_press( const base::point &p, int button ) override;
	bool mouse_move( const base::point &p ) override;
	bool mouse_release( const base::point &p, int button ) override;

	base::signal<void(double)> when_changing;

private:
	void update_value( double v );

	double translate_to_full_w( double v )
	{
		return _min + ( v - x1() ) * ( _max - _min ) / width();
	}

	double translate_from_full_w( double v )
	{
		return x1() + ( v - _min ) * width() / ( _max - _min );
	}

	double translate_to_full_h( double v )
	{
		return _min + ( v - y1() ) * ( _max - _min ) / height();
	}

	double translate_from_full_h( double v )
	{
		return y1() + ( v - _min ) * height() / ( _max - _min );
	}

	double _start = 0.0;

	double _value = 0.0;
	double _handle = 20.0;
	double _page = 0.0;
	double _min = 0.0, _max = 100.0;

	bool _tracking = false;
	bool _horizontal = true;
	bool _bounded = true;

	draw::rectangle _groove;
	draw::rectangle _knob;
};

////////////////////////////////////////

}

