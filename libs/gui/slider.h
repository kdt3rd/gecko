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

class slider_w : public widget
{
public:
	using value_type = coord_type;

	slider_w( void );
	slider_w( value_type v, value_type min = 0.0, value_type max = 1.0 );
	~slider_w( void );

	template<typename T>
	typename std::enable_if<std::is_integral<T>::value, T>::type value( void ) const
	{
		value_type v = _value;
		v = std::max( v, value_type( std::numeric_limits<T>::min() ) );
		v = std::min( v, value_type( std::numeric_limits<T>::max() ) );
		return T( std::round( v ) );
	}

	template<typename T>
	typename std::enable_if<std::is_floating_point<T>::value, T>::type value( void ) const
	{
		value_type v = _value;
		v = std::max( v, value_type( std::numeric_limits<T>::min() ) );
		v = std::min( v, value_type( std::numeric_limits<T>::max() ) );
		return v;
	}

	value_type value( void ) const
	{
		return _value;
	}

	void set_value( value_type v );
	void set_range( value_type min, value_type max );

	void build( gl::api &ogl ) override;
	void paint( gl::api &ogl ) override;

	bool mouse_press( const point &p, int button ) override;
	bool mouse_move( const point &p ) override;
	bool mouse_release( const point &p, int button ) override;

	signal<void(value_type)> when_changing;
	signal<void(value_type)> when_changed;
	signal<void(value_type,value_type)> when_range_changed;

private:
	draw::rectangle _groove;
	draw::shape _knob;

	bool _tracking = false;
	value_type _handle = 8.0;
	value_type _start = 0.0;
	value_type _value = 0.5;
	value_type _min = 0.0, _max = 1.0;
};

////////////////////////////////////////

using slider = widget_ptr<slider_w>;

}

