
#include "color.h"
#include <cmath>
#include <algorithm>
#include <functional>

namespace 
{
	constexpr double PI = 3.14159265358979323846;

	inline double toLin( double c )
	{
		if ( c <= 0.04045 )
			return c / 12.92;
		else
			return pow( ( c + 0.055 ) / ( 1.055 ) , 2.4 );
	}

	inline double fromLin( double c )
	{
		if ( c <= 0.0031308 )
			c = 12.92 * c;
		else
			c = ( 1.055 ) * pow( c, 1.0/2.4 ) - 0.055;

		return std::max( 0.0, std::min( 1.0, c ) );
	}

	inline double labF( double t )
	{                 
		if ( t > std::pow( 6.0/29.0, 3.0 ) )
			return std::pow( t, 1.0/3.0 );
		else    
			return 1.0/3.0 * std::pow( 29.0/6.0, 2.0 ) * t + 4.0/29.0;

	}

	inline double labR( double t )
	{   
		if ( t > 6.0/29.0 )
			return std::pow( t, 3.0 );
		else
			return 3.0 * std::pow( 6.0 / 29.0, 2.0 ) * ( t - 4.0/29.0 );

	}

	// White point
	const float Xn = 0.95047F, Yn = 1.0000F, Zn = 1.08883F;
}

////////////////////////////////////////

namespace base
{

////////////////////////////////////////

void color::get_lab( double &l, double &astar, double &bstar )
{
	double r = toLin( _r );
	double g = toLin( _g );
	double b = toLin( _b );

	double x = 0.4124 * r + 0.3576 * g + 0.1805 * b;
	double y = 0.2126 * r + 0.7152 * g + 0.0722 * b;
	double z = 0.0193 * r + 0.1192 * g + 0.9505 * b;

	l = 1.16 * labF( y / Yn ) - 0.16;
	astar = 5.0 * ( labF( x / Xn ) - labF( y / Yn ) );
	bstar = 2.0 * ( labF( y / Yn ) - labF( z / Zn ) );
}

////////////////////////////////////////

void color::set_lab( double l, double astar, double bstar )
{
	double y = Yn * labR( 1.0/1.16 * ( l + 0.16 ) );
	double x = Xn * labR( 1.0/1.16 * ( l + 0.16 ) + 1.0/5.0 * astar );
	double z = Zn * labR( 1.0/1.16 * ( l + 0.16 ) - 1.0/2.0 * bstar );

	_r = 3.2406 * x - 1.5372 * y - 0.4986 * z;
	_g =-0.9689 * x + 1.8758 * y + 0.0415 * z;
	_b = 0.0557 * x - 0.2040 * y + 1.0570 * z;

	_r = fromLin( _r );
	_g = fromLin( _g );
	_b = fromLin( _b );
}

////////////////////////////////////////

void color::get_hsl( double &h, double &s, double &l )
{
	double min = std::min( std::min( _r, _g ), _b );
	double max = std::max( std::max( _r, _g ), _b );

	l = ( max + min ) / 2;

	if ( std::equal_to<double>()( max, min ) )
		h = s = 0;
	else
	{
		double d = max - min;
		s = l > 0.5 ? ( d / ( 2 - max - min ) ) : ( d / ( max + min ) );
		if ( std::equal_to<double>()( max, _r ) )
			h = ( _g - _b ) / d  + ( _g < _b ? 6 : 0 );
		else if ( std::equal_to<double>()( max, _g ) )
			h = ( _b - _r ) / d + 2;
		else
			h = ( _r - _g ) / d + 4;
	}
	h = ( h / 6.0 ) * 2 * PI;
}

////////////////////////////////////////

namespace {
	double hue2rgb( double p, double q, double t )
	{
		if ( t < 0 )
			t += 1;
		if ( t > 1 )
			t -= 1;
		if ( t < 1.0/6.0 )
			return p + ( q - p ) * 6 * t;
		if ( t < 1.0/2.0 )
			return q;
		if ( t < 2.0/3.0 )
			return p + ( q - p ) * (2.0/3.0 - t) * 6;
		return p;
	}
}

void color::set_hsl( double h, double s, double l )
{
	if ( std::equal_to<double>()( s, 0 ) )
		_r = _g = _b = l;
	else
	{
		double q = l < 0.5 ? ( l * ( 1 + s ) ) : ( l + s - l * s );
		double p = 2 * l - q;
		h = h / ( 2 * PI );
		while ( h < 0 )
			h += 1;
		while ( h > 1 )
			h -= 1;
		_r = hue2rgb( p, q, h + 1.0/3.0 );
		_g = hue2rgb( p, q, h );
		_b = hue2rgb( p, q, h - 1.0/3.0 );
	}

}

////////////////////////////////////////

color color::desaturate( double amt )
{
	double l, a, b;
	get_lab( l, a, b );
	
	return color( space::LAB, l, a * amt, b * amt );
}

////////////////////////////////////////

color color::change( double amt )
{
	double l, a, b;
	get_lab( l, a, b );

	double l1 = std::max( 0.0, std::min( 100.0, l + amt * 100.0 ) );
	double l2 = std::max( 0.0, std::min( 100.0, l - amt * 100.0 ) );

	if ( std::abs( l1 - l ) < std::abs( l2 - l ) * 0.8 )
		l = l2;
	else
		l = l1;

	return color( space::LAB, l, a, b, _a );
}

////////////////////////////////////////

base::color color::mix( const base::color &a, const base::color &b, double m )
{
	double n = 1.0 - m;
	return {
		a.red() * n + b.red() * m,
		a.green() * n + b.green() * m,
		a.blue() * n + b.blue() * m,
		a.alpha() * n + b.alpha() * m
	};
}


////////////////////////////////////////

}

