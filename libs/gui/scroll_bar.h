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
	scroll_bar( void );
	~scroll_bar( void );

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
	void set_range( double min, double max );

	void build( gl::api &ogl ) override;
	void paint( gl::api &ogl ) override;

	bool mouse_press( const base::point &p, int button ) override;
	bool mouse_move( const base::point &p ) override;
	bool mouse_release( const base::point &p, int button ) override;

	base::signal<void(double)> when_changing;

private:
	void fix_value( void );

	double translate_to_full_w( double v )
	{
		return ( v - x1() ) * ( _max - _min ) / width();
	}

	double translate_from_full_w( double v )
	{
		return x1() + v * width() / ( _max - _min );
	}

	double translate_to_full_h( double v )
	{
		return ( v - y1() ) * ( _max - _min ) / height();
	}

	double translate_from_full_h( double v )
	{
		return y1() + v * height() / ( _max - _min );
	}

	double _start = 0.0;

	double _value = 45.0;
	double _handle = 10.0;
	double _min = 0.0, _max = 100.0;

	bool _tracking = false;
	bool _horizontal = true;

	draw::rectangle _groove;
	draw::rectangle _knob;
};

////////////////////////////////////////

}

