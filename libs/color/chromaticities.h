//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <initializer_list>
#include <base/contract.h>

#include "chroma_coord.h"
#include "matrix.h"


////////////////////////////////////////


namespace color
{

enum class cone_response
{
	XYZ, ///< no-op
	HPE, ///< Hunt, Pointer, Estevez Equal Energy
	BRADFORD, ///< Bradford
	VON_KRIES, ///< Hunt, Pointer, Estevez D65 norm
	CIECAM97s, ///< Fairchild updated CIECAM97s
	SHARP, ///< Sharpened cone response
	CMCCAT2000, ///< fit from all available color data sets
	CIECAM02, ///< CAT02,
	WANDELL, ///< LMS from Wandell, Foundations of Vision
	NONE, ///< disabled
	DEFAULT = BRADFORD
};

template <typename V>
inline matrix<V> to_lms( cone_response cr = cone_response::DEFAULT )
{
	typedef matrix<V> mat;
	mat Ma;
	switch ( cr )
	{
		case cone_response::XYZ:
			// leave at identity
			break;
		case cone_response::HPE:
			Ma = mat( V(0.38971), V(0.68898), V(-0.07868),
					  V(-0.22981), V(1.18340), V(0.04641),
					  V(0.0), V(0.0), V(1.0) );
			break;
		case cone_response::BRADFORD:
			Ma = mat( V(0.8951), V(0.2664), V(-0.1614),
					  V(-0.7502), V(1.7135), V(0.0367),
					  V(0.0389), V(-0.0685), V(1.0296) );
			break;
		case cone_response::VON_KRIES: // normalized to d65...
			// TODO: if I normalize the H-P-E LMS by diag(1/d65), I get close to this
			// but decidedly not this...
			Ma = mat( V(0.40024), V(0.7076), V(-0.08081),
					  V(-0.2263), V(1.16532), V(0.0457),
					  V(0.0), V(0.0), V(0.91822) );
			break;
		case cone_response::CIECAM97s:
			Ma = mat( V(0.8562), V(0.3372), V(-0.1934),
					  V(-0.836), V(1.8327), V(0.0033),
					  V(0.0357), V(-0.0469), V(1.0112) );
			break;
		case cone_response::SHARP:
			Ma = mat( V(1.2694), V(-0.0988), V(-0.1706),
					  V(-0.8364), V(1.8006), V(0.0357),
					  V(0.0297), V(-0.0315), V(1.0018) );
			break;
		case cone_response::CMCCAT2000:
			Ma = mat( V(0.7982), V(0.3389), V(-0.1371),
					  V(-0.5918), V(1.5512), V(0.0406),
					  V(0.0008), V(0.239), V(0.9753) );
			break;
		case cone_response::CIECAM02:
			Ma = mat( V(0.7328), V(0.4296), V(-0.1624),
					  V(-0.7036), V(1.6975), V(0.0061),
					  V(0.003), V(0.0136), V(0.9834) );
			break;
		case cone_response::WANDELL:
			Ma = mat( V(0.2430), V(0.8560), V(-0.0440),
					  V(-0.3910), V(1.1650), V(0.0870),
					  V(0.0100), V(-0.0080), V(0.5630) );
			break;
		case cone_response::NONE:
			throw std::runtime_error( "Unhandled white point cone response method" );
	}

	return Ma;
}

////////////////////////////////////////

template <typename V>
inline matrix<V> from_lms( cone_response cr = cone_response::DEFAULT )
{
	return to_lms<V>( cr ).invert();
}


template <typename V>
inline triplet<V> norm_white( cone_response cr = cone_response::DEFAULT, V Y = V(1) )
{
	using cx = chroma_coord<V>;
	using xyz = triplet<V>;
	switch ( cr )
	{
		case cone_response::XYZ:
		case cone_response::HPE:
		case cone_response::BRADFORD:
		case cone_response::CIECAM97s:
		case cone_response::SHARP:
		case cone_response::CMCCAT2000:
		case cone_response::CIECAM02:
		case cone_response::WANDELL:
			break;
		case cone_response::VON_KRIES: // normalized to d65...
			return cx( V(0.31271), V(0.32902) ).toXYZ( Y );
		case cone_response::NONE:
			throw std::runtime_error( "Unhandled white point cone response method" );
	}

	return xyz( Y );
}

///
/// @brief Class chromaticities provides a container to store red, green, blue, and white pairs
///
template <typename V>
class chromaticities
{
	static_assert( std::is_floating_point<V>::value, "color::chromaticities requires floating point type" );
public:
	typedef V value_type;
	typedef chroma_coord<value_type> xy;
	typedef triplet<value_type> xyz;
	typedef matrix<value_type> mat;

	constexpr chromaticities( void ) noexcept = default;
	constexpr chromaticities( const chromaticities & ) noexcept = default;
	chromaticities &operator=( const chromaticities & ) noexcept = default;
	constexpr chromaticities( chromaticities && ) noexcept = default;
	chromaticities &operator=( chromaticities && ) noexcept = default;
	~chromaticities( void ) = default;

	inline constexpr chromaticities( const xy &r, const xy &g, const xy &b, const xy &w )
		 noexcept(std::is_nothrow_default_constructible<value_type>::value)
		: red( r ), green( g ), blue( b ), white( w ) {}

	inline chromaticities( std::initializer_list<xy> &i )
	{
		if ( i.size() != 4 )
			throw_runtime( "Invalid initializer list for chromaticities, expect 4 x,y chroma pairs" );
		red = i[0];
		green = i[1];
		blue = i[2];
		white = i[3];
	}

	inline mat RGBtoXYZ( value_type Y = value_type(1) ) const
	{
		xyz r = red.toXYZ( Y );
		xyz g = green.toXYZ( Y );
		xyz b = blue.toXYZ( Y );
		xyz w = white.toXYZ( Y );

		xyz s = mat( r, g, b ).invert() * w;
		return mat( s.x * r.x, s.y * g.x, s.z * b.x,
					s.x * r.y, s.y * g.y, s.z * b.y,
					s.x * r.z, s.y * g.z, s.z * b.z );
	}

	inline mat XYZtoRGB( value_type Y = value_type(1) ) const
	{
		return RGBtoXYZ( Y ).invert();
	}

	inline mat XYZtoLMS( cone_response cr, value_type Y = value_type(1) ) const
	{
		mat m = to_lms<value_type>( cr );
		// now re-scale based on the white point
		xyz w = white.toXYZ( Y );
		xyz nw = norm_white<value_type>( cr, Y );
		return ( w != nw ) ? mat::diag( xyz( value_type(1) ) / ( m * ( w / nw ) ) ) * m : m;
	}

	inline mat LMStoXYZ( cone_response cr, value_type Y = value_type(1) ) const
	{
		return XYZtoLMS( cr, Y ).invert();
	}

	/// Computes the adaptation matrix from o to this
	inline mat adaptation( const chromaticities &o, value_type amount = value_type(1.0), cone_response m = cone_response::DEFAULT ) const
	{
		amount = std::max( value_type(0.0), std::min( value_type(1.0), amount ) );

		if ( m == cone_response::NONE || white == o.white || amount <= value_type(0.0) )
			return mat();

		mat Ma = to_lms<value_type>( m );
		mat MaInv = Ma.invert();
		xyz rhoWsrc = Ma * o.white.toXYZ();
		xyz rhoWdst = Ma * white.toXYZ();

		mat scale = mat::diag( ( rhoWdst.x / rhoWsrc.x ) * amount + value_type(1.0) - amount,
							   ( rhoWdst.y / rhoWsrc.y ) * amount + value_type(1.0) - amount,
							   ( rhoWdst.z / rhoWsrc.z ) * amount + value_type(1.0) - amount );

		return MaInv * scale * Ma;
	}

	/// Computes the conversion matrix from o to this
	inline mat conversion( const chromaticities &o,
						   value_type adapt_amount = value_type(0.0),
						   cone_response m = cone_response::DEFAULT )
	{
		return XYZtoRGB() * adaptation( o, adapt_amount, m ) * o.RGBtoXYZ();
	}

	xy red = xy( value_type(1), value_type(0) );
	xy green = xy( value_type(1), value_type(1) );
	xy blue = xy( value_type(0), value_type(1) );
	xy white = xy( value_type(1) / value_type(3), value_type(1) / value_type(3) );
};

} // namespace color



