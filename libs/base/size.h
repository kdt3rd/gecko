
#pragma once

#include <iostream>
#include <cmath>

namespace base
{

////////////////////////////////////////

/// @brief Width and height
class size
{
public:
	/// @brief Default constructor
	constexpr size( void )
	{
	}

	/// @brief Constructor with width and height
	constexpr size( double ww, double hh )
		: _w( ww ), _h( hh )
	{
	}

	/// @brief Width
	constexpr double w( void ) const { return _w; }

	/// @brief Height
	constexpr double h( void ) const { return _h; }

	/// @brief Set the width and height
	void set( double ww, double hh )
	{
		_w = ww;
		_h = hh;
	}

	/// @brief Set the width
	void set_width( double ww ) { _w = ww; }

	/// @brief Set the height
	void set_height( double hh ) { _h = hh; }

	/// @brief Shrink width and height
	void shrink( double dw, double dh ) { _w -= dw; _h -= dh; }

	/// @brief Grow width and height
	void grow( double dw, double dh ) { _w += dw; _h += dh; }

	/// @brief Add two sizes together
	size operator+( const size &s ) const
	{
		return { _w + s._w, _h + s._h };
	}

	/// @brief Add two sizes together
	bool operator<( const size &o ) const
	{
		return _w < o._w || ( _w == o._w && _h < o._h );
	}

	/// @brief Round size up to nearest integer
	void ceil( void )
	{
		_w = std::ceil( _w );
		_h = std::ceil( _h );
	}

	/// @brief Round size down to nearest integer
	void floor( void )
	{
		_w = std::floor( _w );
		_h = std::floor( _h );
	}

private:
	double _w = 0.0, _h = 0.0;
};

/// @brief Stream out a size object
inline std::ostream &operator<<( std::ostream &out, const size &s )
{
	out << s.w() << 'x' << s.h();
	return out;
}

////////////////////////////////////////

}

