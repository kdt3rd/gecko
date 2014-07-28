
#pragma once

#include <iostream>

namespace base
{

////////////////////////////////////////


/// @brief A color
///
/// Color represented by red, green, blue, and alpha.
/// Typical colors are between 0.0 and 1.0.
class color
{
public:
	/// @brief Color space
	enum class space
	{
		SRGB, // R G B [0..1]
		LAB, // L [0..1], a* b* [-1,1]
		HSL, // H radians, S L [0..1]
	};

	/// @brief Default constructor
	constexpr color( void )
	{
	}

	/// @brief Construct an RGBA color with the given values
	constexpr color( double r, double g, double b, double a = 1.0 )
		: _r( r ), _g( g ), _b( b ), _a( a )
	{
	}

	/// @brief Construct a color with the given space and values
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

	/// @brief Red value of this color
	constexpr double red( void ) const { return _r; }

	/// @brief Green value of this color
	constexpr double green( void ) const { return _g; }

	/// @brief Blue value of this color
	constexpr double blue( void ) const { return _b; }

	/// @brief Alpha value of this color
	constexpr double alpha( void ) const { return _a; }

	/// @brief Set the color with the given values
	void set( double r, double g, double b, double a = 1.0 )
	{
		_r = r;
		_g = g;
		_b = b;
		_a = a;
	}

	/// @brief Get the LAB color values
	void get_lab( double &l, double &a, double &b );

	/// @brief Set the color values in the LAB color space
	void set_lab( double l, double a, double b );

	/// @brief Get the HSL color values
	void get_hsl( double &h, double &s, double &l );

	/// @brief Set the color values in the HSL color space
	void set_hsl( double h, double s, double l );

	/// @brief Desaturate the color
	///
	/// The amount should be between 0.0 and 1.0.
	/// Amounts greater than 1.0 will add saturation.
	color desaturate( double amt );

	/// @brief Change the color
	///
	/// Change the color by the given amount.
	/// This will make the color brighter or darker.
	/// The amount should be between 0.0 and 1.0.
	color change( double amt );

	/// @brief Mix two colors together
	static color mix( const color &a, const color &b, double m = 0.5 );

private:
	double _r = 0.0, _g = 0.0, _b = 0.0, _a = 0.0;
};

////////////////////////////////////////

constexpr color white( 1, 1, 1 );
constexpr color black( 0, 0, 0 );
constexpr color red( 1, 0, 0 );
constexpr color green( 0, 1, 0 );
constexpr color blue( 0, 0, 1 );


////////////////////////////////////////

/// @brief Stream out a color
inline std::ostream &operator<<( std::ostream &out, const color &c )
{
	out << c.red() << ',' << c.green() << ',' << c.blue() << ',' << c.alpha();
	return out;
}

////////////////////////////////////////

}

