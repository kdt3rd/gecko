//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#pragma once

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
			Ma = mat( 0.38971, 0.68898, -0.07868,
					  -0.22981, 1.18340, 0.04641,
					  0.0, 0.0, 1.0 );
			break;
		case cone_response::BRADFORD:
			Ma = mat( 0.8951, 0.2664, -0.1614,
					  -0.7502, 1.7135, 0.0367,
					  0.0389, -0.0685, 1.0296 );
			break;
		case cone_response::VON_KRIES:
			Ma = mat( 0.40024, 0.7076, -0.08081,
					  -0.2263, 1.16532, 0.0457,
					  0.0, 0.0, 0.91822 );
			break;
		case cone_response::CIECAM97s:
			Ma = mat( 0.8562, 0.3372, -0.1934,
					  -0.836, 1.8327, 0.0033,
					  0.0357, -0.0469, 1.0112 );
			break;
		case cone_response::SHARP:
			Ma = mat( 1.2694, -0.0988, -0.1706,
					  -0.8364, 1.8006, 0.0357,
					  0.0297, -0.0315, 1.0018 );
			break;
		case cone_response::CMCCAT2000:
			Ma = mat( 0.7982, 0.3389, -0.1371,
					  -0.5918, 1.5512, 0.0406,
					  0.0008, 0.239, 0.9753 );
			break;
		case cone_response::CIECAM02:
			Ma = mat( 0.7328, 0.4296, -0.1624,
					  -0.7036, 1.6975, 0.0061,
					  0.003, 0.0136, 0.9834 );
			break;
		default:
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

	inline mat XYZtoRGB( value_type Y = value_type(1) )
	{
		return RGBtoXYZ( Y ).invert();
	}

	/// Computes the adaptation matrix from o to this
	inline mat adaptation( const chromaticities &o, cone_response m = cone_response::DEFAULT )
	{
		if ( m == cone_response::NONE || white == o.white )
			return mat();

		mat Ma = to_lms<value_type>( m );
		mat MaInv = Ma.invert();
		xyz rhoWsrc = Ma * o.white.toXYZ();
		xyz rhoWdst = Ma * white.toXYZ();

		mat scale = mat::diag( rhoWdst.x / rhoWsrc.x,
							   rhoWdst.y / rhoWsrc.y,
							   rhoWdst.z / rhoWsrc.z );
		return MaInv * scale * Ma;
	}

	/// Computes the conversion matrix from o to this
	inline mat conversion( const chromaticities &o,
						   cone_response m = cone_response::DEFAULT )
	{
		return XYZtoRGB() * adaptation( o, m ) * o.RGBtoXYZ();
	}
	xy red, green, blue, white;
};

} // namespace color


