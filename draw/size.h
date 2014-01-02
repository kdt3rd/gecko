
#pragma once

#include <iostream>
#include <cmath>

namespace draw
{

////////////////////////////////////////

/// @brief Point.
class size
{
public:
	constexpr size( void )
	{
	}

	constexpr size( double ww, double hh )
		: _w( ww ), _h( hh )
	{
	}

	constexpr double w( void ) const { return _w; }
	constexpr double h( void ) const { return _h; }

	void set( double ww, double hh )
	{
		_w = ww;
		_h = hh;
	}

	void set_width( double ww ) { _w = ww; }
	void set_height( double hh ) { _h = hh; }

	void shrink( double dw, double dh ) { _w -= dw; _h -= dh; }
	void grow( double dw, double dh ) { _w += dw; _h += dh; }

	size operator+( const size &s ) const
	{
		return { _w + s._w, _h + s._h };
	}

	void ceil( void )
	{
		_w = std::ceil( _w );
		_h = std::ceil( _h );
	}

	void floor( void )
	{
		_w = std::floor( _w );
		_h = std::floor( _h );
	}

private:
	double _w = 0.0, _h = 0.0;
};

inline std::ostream &operator<<( std::ostream &out, const size &s )
{
	out << s.w() << 'x' << s.h();
	return out;
}

////////////////////////////////////////

}

