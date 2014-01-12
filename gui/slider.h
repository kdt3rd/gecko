
#pragma once

#include "widget.h"
#include <model/datum.h>

namespace gui
{

////////////////////////////////////////

class slider : public widget
{
public:
	slider( void );
	slider( datum<double> &&v, datum<double> &&min = 0.0, datum<double> &&max = 1.0 );
	~slider( void );

	template<typename T>
	typename std::enable_if<std::is_integral<T>::value, T>::type value( void ) const
	{
		double v = _value.value();
		v = std::max( v, double( std::numeric_limits<T>::min() ) );
		v = std::min( v, double( std::numeric_limits<T>::max() ) );
		return T( std::round( v ) );
	}

	template<typename T>
	typename std::enable_if<std::is_floating_point<T>::value, T>::type value( void ) const
	{
		double v = _value.value();
		v = std::max( v, double( std::numeric_limits<T>::min() ) );
		v = std::min( v, double( std::numeric_limits<T>::max() ) );
		return v;
	}

	double value( void ) const
	{
		return _value.value();
	}

	void set_value( double v );
	void set_range( double min, double max );

	void paint( const std::shared_ptr<gldraw::canvas> &c ) override;

	void compute_minimum( void ) override;

	bool mouse_press( const core::point &p, int button ) override;
	bool mouse_move( const core::point &p ) override;
	bool mouse_release( const core::point &p, int button ) override;

private:
	bool _tracking = false;
	bool _pressed = false;
	double _handle = 12.0;
	double _start = 0.0;
	datum<double> _value = 0.5;
	datum<double> _min = 0.0, _max = 1.0;
};

////////////////////////////////////////

}

