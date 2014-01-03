
#pragma once

namespace draw
{

////////////////////////////////////////


/// @brief A color.
///
/// Color represented by red, green, blue, and alpha.
/// Valid colors should be between 0 and 1.
class color
{
public:
	enum class space
	{
		SRGB,
		LAB,
		HSL,
	};

	constexpr color( void )
	{
	}

	constexpr color( double r, double g, double b, double a = 1.0 )
		: _r( r ), _g( g ), _b( b ), _a( a )
	{
	}

	color( space s, double i, double j, double k, double a = 1.0 )
		: _a( a )
	{
		switch ( s )
		{
			case space::SRGB: set( i, j, k ); break;
			case space::LAB: set_lab( i, j, k ); break;
			case space::HSL: set_hsl( i, j, k ); break;
		}
	}

	constexpr double red( void ) const { return _r; }
	constexpr double green( void ) const { return _g; }
	constexpr double blue( void ) const { return _b; }
	constexpr double alpha( void ) const { return _a; }

	void set( double r, double g, double b, double a = 1.0 )
	{
		_r = r;
		_g = g;
		_b = b;
		_a = a;
	}

	void get_lab( double &l, double &a, double &b );
	void set_lab( double l, double a, double b );

	void get_hsl( double &h, double &s, double &l );
	void set_hsl( double h, double s, double l );

	color desaturate( double amt );
	color change( double amt );

private:
	double _r = 0.0, _g = 0.0, _b = 0.0, _a = 0.0;
};

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const color &c )
{
	out << c.red() << ',' << c.green() << ',' << c.blue() << ',' << c.alpha();
	return out;
}

////////////////////////////////////////

}

