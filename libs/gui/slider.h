//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "widget.h"
#include <draw/rectangle.h>
#include <draw/shape.h>
#include <base/signal.h>

namespace gui
{

////////////////////////////////////////

class slider : public widget
{
public:
	slider( void );
	slider( double v, double min = 0.0, double max = 1.0 );
	~slider( void );

	template<typename T>
	typename std::enable_if<std::is_integral<T>::value, T>::type value( void ) const
	{
		double v = _value;
		v = std::max( v, double( std::numeric_limits<T>::min() ) );
		v = std::min( v, double( std::numeric_limits<T>::max() ) );
		return T( std::round( v ) );
	}

	template<typename T>
	typename std::enable_if<std::is_floating_point<T>::value, T>::type value( void ) const
	{
		double v = _value;
		v = std::max( v, double( std::numeric_limits<T>::min() ) );
		v = std::min( v, double( std::numeric_limits<T>::max() ) );
		return v;
	}

	double value( void ) const
	{
		return _value;
	}

	void set_value( double v );
	void set_range( double min, double max );

	void build( gl::api &ogl ) override;
	void paint( gl::api &ogl ) override;

	void compute_bounds( void ) override;

	bool mouse_press( const base::point &p, int button ) override;
	bool mouse_move( const base::point &p ) override;
	bool mouse_release( const base::point &p, int button ) override;

	base::signal<void(double)> when_changing;
	base::signal<void(double)> when_changed;
	base::signal<void(double,double)> when_range_changed;

private:
	draw::rectangle _groove;
	draw::shape _knob;

	bool _tracking = false;
	bool _pressed = false;
	double _handle = 8.0;
	double _start = 0.0;
	double _value = 0.5;
	double _min = 0.0, _max = 1.0;
};

////////////////////////////////////////

}

