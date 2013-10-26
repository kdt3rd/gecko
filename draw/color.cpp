
#include "color.h"
#include <cmath>
#include <algorithm>

////////////////////////////////////////

namespace
{
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

namespace draw
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

	l = 116.0 * labF( y / Yn ) - 16.0;
	astar = 500.0 * ( labF( x / Xn ) - labF( y / Yn ) );
	bstar = 200.0 * ( labF( y / Yn ) - labF( z / Zn ) );
}

////////////////////////////////////////

void color::set_lab( double l, double astar, double bstar )
{
	double y = Yn * labR( 1.0/116.0 * ( l + 16.0 ) );
	double x = Xn * labR( 1.0/116.0 * ( l + 16.0 ) + 1.0/500.0 * astar );
	double z = Zn * labR( 1.0/116.0 * ( l + 16.0 ) - 1.0/200.0 * bstar );

	double r = 3.2406 * x - 1.5372 * y - 0.4986 * z;
	double g =-0.9689 * x + 1.8758 * y + 0.0415 * z;
	double b = 0.0557 * x - 0.2040 * y + 1.0570 * z;

	_r = fromLin( r );
	_g = fromLin( g );
	_b = fromLin( b );
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

}

