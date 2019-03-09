// Copyright (c) 2016 Ian Godin
// SPDX-License-Identifier: MIT

#include "color.h"
#include <base/math_functions.h>
#include <cmath>
#include <algorithm>
#include <functional>

namespace
{
	constexpr float PI = 3.14159265358979F;

	inline float toLin( float c )
	{
		if ( c <= 0.04045F )
			return c / 12.92F;
		else
			return std::pow( ( c + 0.055F ) / ( 1.055F ) , 2.4F );
	}

	inline float fromLin( float c )
	{
		if ( c <= 0.0031308F )
			c = 12.92F * c;
		else
			c = 1.055F * std::pow( c, 1.0F/2.4F ) - 0.055F;

		return std::max( 0.0F, std::min( 1.0F, c ) );
	}

	inline float labF( float t )
	{
		if ( t > std::pow( 6.0F/29.0F, 3.0F ) )
			return std::pow( t, 1.0F/3.0F );
		else
			return 1.0F/3.0F * std::pow( 29.0F/6.0F, 2.0F ) * t + 4.0F/29.0F;

	}

	inline float labR( float t )
	{
		if ( t > 6.0F/29.0F )
			return std::pow( t, 3.0F );
		else
			return 3.0F * std::pow( 6.0F / 29.0F, 2.0F ) * ( t - 4.0F/29.0F );

	}

	// White point
	const float Xn = 0.95047F, Yn = 1.0000F, Zn = 1.08883F;
}

////////////////////////////////////////

namespace gl
{

////////////////////////////////////////

void color::get_srgb( float &r, float &g, float &b ) const
{
	r = _r;
	g = _g;
	b = _b;
}

////////////////////////////////////////

void color::set_srgb( float r, float g, float b )
{
	_r = r;
	_g = g;
	_b = b;
}

////////////////////////////////////////

void color::get_lin( float &r, float &g, float &b ) const
{
	r = toLin( _r );
	g = toLin( _g );
	b = toLin( _b );
}

////////////////////////////////////////

void color::set_lin( float r, float g, float b )
{
	_r = fromLin( r );
	_g = fromLin( g );
	_b = fromLin( b );
}

////////////////////////////////////////

void color::get_lab( float &l, float &astar, float &bstar ) const
{
	float r = toLin( _r );
	float g = toLin( _g );
	float b = toLin( _b );

	float x = 0.4124F * r + 0.3576F * g + 0.1805F * b;
	float y = 0.2126F * r + 0.7152F * g + 0.0722F * b;
	float z = 0.0193F * r + 0.1192F * g + 0.9505F * b;

	l = 1.16F * labF( y / Yn ) - 0.16F;
	astar = 5.0F * ( labF( x / Xn ) - labF( y / Yn ) );
	bstar = 2.0F * ( labF( y / Yn ) - labF( z / Zn ) );
}

////////////////////////////////////////

void color::set_lab( float l, float astar, float bstar )
{
	float y = Yn * labR( 1.0F/1.16F * ( l + 0.16F ) );
	float x = Xn * labR( 1.0F/1.16F * ( l + 0.16F ) + 1.0F/5.0F * astar );
	float z = Zn * labR( 1.0F/1.16F * ( l + 0.16F ) - 1.0F/2.0F * bstar );

	_r = 3.2406F * x - 1.5372F * y - 0.4986F * z;
	_g =-0.9689F * x + 1.8758F * y + 0.0415F * z;
	_b = 0.0557F * x - 0.2040F * y + 1.0570F * z;

	_r = fromLin( _r );
	_g = fromLin( _g );
	_b = fromLin( _b );
}

////////////////////////////////////////

void color::get_hsl( float &h, float &s, float &l ) const
{
	float min = std::min( std::min( _r, _g ), _b );
	float max = std::max( std::max( _r, _g ), _b );

	l = ( max + min ) / 2.0F;

	if ( std::equal_to<float>()( max, min ) )
		h = s = 0.0F;
	else
	{
		float d = max - min;
		s = d / ( 1.F - std::abs( 2.F * l - 1.F ) );
		if ( std::equal_to<float>()( max, _r ) )
			h = ( _g - _b ) / d  + ( _g < _b ? 6.0F : 0.0F );
		else if ( std::equal_to<float>()( max, _g ) )
			h = ( _b - _r ) / d + 2.0F;
		else
			h = ( _r - _g ) / d + 4.0F;
	}
	h = ( h / 6.0F ) * 2.0F * PI;
}

////////////////////////////////////////

namespace {
	float hue2rgb( float p, float q, float t )
	{
		if ( t < 0.F )
			t += 1.F;
		if ( t > 1.F )
			t -= 1.F;
		if ( t < 1.0F/6.0F )
			return p + ( q - p ) * 6.0F * t;
		if ( t < 1.0F/2.0F )
			return q;
		if ( t < 2.0F/3.0F )
			return p + ( q - p ) * (2.0F/3.0F - t) * 6.0F;
		return p;
	}
}

void color::set_hsl( float h, float s, float l )
{
	if ( std::equal_to<float>()( s, 0.0F ) )
		_r = _g = _b = l;
	else
	{
		float q = l < 0.5F ? ( l * ( 1.0F + s ) ) : ( l + s - l * s );
		float p = 2.0F * l - q;
		h = h / ( 2.0F * PI );
		while ( h < 0.0F )
			h += 1.0F;
		while ( h > 1.0F )
			h -= 1.0F;
		_r = hue2rgb( p, q, h + 1.0F/3.0F );
		_g = hue2rgb( p, q, h );
		_b = hue2rgb( p, q, h - 1.0F/3.0F );
	}

}

////////////////////////////////////////

float color::relative_luminance( void ) const
{
	float r, g, b;
	get_lin( r, g, b );
	return std::max( 0.F, 0.2126F * r + 0.7152F * g + 0.0722F * b );
}

////////////////////////////////////////

float color::contrast_ratio( const gl::color &c2 ) const
{
	float l1 = relative_luminance() + 0.005F;
	float l2 = c2.relative_luminance() + 0.005F;
	if ( l1 < l2 )
		std::swap( l1, l2 );
	return l1 / l2;
}

////////////////////////////////////////

float color::distance( const gl::color &c2 ) const
{
	float l1, a1, b1;
	get_lab( l1, a1, b1 );
	float l2, a2, b2;
	c2.get_lab( l2, a2, b2 );

	float ld = l1 - l2;
	float ad = a1 - a2;
	float bd = b1 - b2;

	return std::sqrt( ld*ld + ad*ad + bd*bd );
}

////////////////////////////////////////

color color::desaturate( float amt )
{
	float l, a, b;
	get_lab( l, a, b );

	return color( space::LAB, l, a * amt, b * amt );
}

////////////////////////////////////////

color color::change( float amt )
{
	float l, a, b;
	get_lab( l, a, b );

	float l1 = std::max( 0.0F, std::min( 100.0F, l + amt * 100.0F ) );
	float l2 = std::max( 0.0F, std::min( 100.0F, l - amt * 100.0F ) );

	if ( std::abs( l1 - l ) < std::abs( l2 - l ) * 0.8F )
		l = l2;
	else
		l = l1;

	return color( space::LAB, l, a, b, _a );
}

////////////////////////////////////////

color color::mix( const color &a, const color &b, float m )
{
	float n = 1.0F - m;
	float ra, ga, ba;
	float rb, gb, bb;
	a.get_lin( ra, ga, ba );
	b.get_lin( rb, gb, bb );
	return {
		space::LINEAR,
		ra * n + rb * m,
		ga * n + gb * m,
		ba * n + bb * m,
		a.alpha() * n + b.alpha() * m
	};
}

////////////////////////////////////////

color color::mix_rgb( const color &a, const color &b, float m )
{
	float n = 1.0F - m;
	float ra, ga, ba;
	float rb, gb, bb;

	a.get_srgb( ra, ga, ba );
	b.get_srgb( rb, gb, bb );
	return {
		space::SRGB,
		ra * n + rb * m,
		ga * n + gb * m,
		ba * n + bb * m,
		a.alpha() * n + b.alpha() * m
	};
}

////////////////////////////////////////

}

