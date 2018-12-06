//
// Copyright (c) 2014-2016 Ian Godin
// SPDX-License-Identifier: MIT
//

#pragma once

#include <iostream>

namespace gl
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
		LINEAR, // R G B [0..1]
		LAB, // L [0..1], a* b* [-1,1]
		HSL, // H radians, S L [0..1]
	};

	/// @brief Default constructor
	constexpr color( void )
	{
	}

	/// @brief Construct an RGBA color with the given values
	constexpr color( float r, float g, float b, float a = 1.0 )
		: _r( r ), _g( g ), _b( b ), _a( a )
	{
	}

	/// @brief Construct a color with the given space and values
	color( space s, float i, float j, float k, float a = 1.0 )
		: _a( a )
	{
		switch ( s )
		{
			case space::SRGB: set( i, j, k ); break;
			case space::LINEAR: set_lin( i, j, k ); break;
			case space::LAB: set_lab( i, j, k ); break;
			case space::HSL: set_hsl( i, j, k ); break;
		}
	}

	/// @brief Red value of this color
	constexpr float red( void ) const { return _r; }

	/// @brief Green value of this color
	constexpr float green( void ) const { return _g; }

	/// @brief Blue value of this color
	constexpr float blue( void ) const { return _b; }

	/// @brief Alpha value of this color
	constexpr float alpha( void ) const { return _a; }

	/// @brief Set the color with the given values
	void set( float r, float g, float b, float a = 1.0 )
	{
		_r = r;
		_g = g;
		_b = b;
		_a = a;
	}

	/// @brief Get the srgb color values
	void get_srgb( float &r, float &g, float &b ) const;

	/// @brief Set the color values in the srgb color space
	void set_srgb( float r, float g, float b );

	/// @brief Get the linear color values
	void get_lin( float &r, float &g, float &b ) const;

	/// @brief Set the color values in the linear color space
	void set_lin( float r, float g, float b );

	/// @brief Get the LAB color values
	void get_lab( float &l, float &a, float &b ) const;

	/// @brief Set the color values in the LAB color space
	void set_lab( float l, float a, float b );

	/// @brief Get the HSL color values
	void get_hsl( float &h, float &s, float &l ) const;

	/// @brief Set the color values in the HSL color space
	void set_hsl( float h, float s, float l );

	/// @brief Get the relative luminance
	float relative_luminance( void ) const;

	/// @brief Get the contrast ratio between 2 colors.
	float contrast_ratio( const gl::color &c2 ) const;

	float distance( const gl::color &c2 ) const;

	/// @brief Desaturate the color
	///
	/// The amount should be between 0.0 and 1.0.
	/// Amounts greater than 1.0 will add saturation.
	color desaturate( float amt );

	/// @brief Change the color
	///
	/// Change the color by the given amount.
	/// This will make the color brighter or darker.
	/// The amount should be between 0.0 and 1.0.
	color change( float amt );

	/// @brief Mix two colors together
	static color mix( const color &a, const color &b, float m = 0.5 );

	/// @brief Mix two colors together
	static color mix_rgb( const color &a, const color &b, float m = 0.5 );

	static gl::color make8( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 )
	{
		return gl::color( r / 255.F, g / 255.F, b / 255.F, a / 255.F );
	}

private:
	float _r = 0.0, _g = 0.0, _b = 0.0, _a = 0.0;
};

////////////////////////////////////////

constexpr color transparent( 0.F, 0.F, 0.F, 0.F );
constexpr color white( 1.F, 1.F, 1.F );
constexpr color black( 0.F, 0.F, 0.F );
constexpr color grey( 0.5F, 0.5F, 0.5F );
constexpr color gray( 0.5F, 0.5F, 0.5F );
constexpr color red( 1.F, 0.F, 0.F );
constexpr color green( 0.F, 1.F, 0.F );
constexpr color blue( 0.F, 0.F, 1.F );
constexpr color yellow( 1.F, 1.F, 0.F );
constexpr color magenta( 1.F, 0.F, 1.F );
constexpr color cyan( 0.F, 1.F, 1.F );

////////////////////////////////////////

/// @brief Stream out a color
inline std::ostream &operator<<( std::ostream &out, const color &c )
{
	out << c.red() << ',' << c.green() << ',' << c.blue() << ',' << c.alpha();
	return out;
}

////////////////////////////////////////

}

