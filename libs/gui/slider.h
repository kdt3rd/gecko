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
	using slider_value = coord_type;

	slider( void );
	slider( slider_value v, slider_value min = 0.0, slider_value max = 1.0 );
	~slider( void );

	template<typename T>
	typename std::enable_if<std::is_integral<T>::value, T>::type value( void ) const
	{
		slider_value v = _value;
		v = std::max( v, slider_value( std::numeric_limits<T>::min() ) );
		v = std::min( v, slider_value( std::numeric_limits<T>::max() ) );
		return T( std::round( v ) );
	}

	template<typename T>
	typename std::enable_if<std::is_floating_point<T>::value, T>::type value( void ) const
	{
		slider_value v = _value;
		v = std::max( v, slider_value( std::numeric_limits<T>::min() ) );
		v = std::min( v, slider_value( std::numeric_limits<T>::max() ) );
		return v;
	}

	slider_value value( void ) const
	{
		return _value;
	}

	void set_value( slider_value v );
	void set_range( slider_value min, slider_value max );

	void build( gl::api &ogl ) override;
	void paint( gl::api &ogl ) override;

	bool mouse_press( const point &p, int button ) override;
	bool mouse_move( const point &p ) override;
	bool mouse_release( const point &p, int button ) override;

	signal<void(slider_value)> when_changing;
	signal<void(slider_value)> when_changed;
	signal<void(slider_value,slider_value)> when_range_changed;

private:
	draw::rectangle _groove;
	draw::shape _knob;

	bool _tracking = false;
	slider_value _handle = 8.0;
	slider_value _start = 0.0;
	slider_value _value = 0.5;
	slider_value _min = 0.0, _max = 1.0;
};

////////////////////////////////////////

}

