//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <base/compiler_support.h>

#include "chromaticities.h"
#include "primaries.h"
#include "space.h"
#include "range.h"
#include "transfer_curve.h"


////////////////////////////////////////


namespace color
{


/// @brief Class description provides a basic set of attributes
///        describing the current state of an image or other color
///        value.
///
/// This is most commonly used to describe how to interpret color channels
/// of an image, and then can be used to compute the transformation between
/// different representations.
///
/// There are a few basic attributes that determine this:
/// - the underlying chromaticities of the volume involved
///   (for describing the RGB<->XYZ tranformation)
/// - diffuse / reference white scale value.
///   NB: This is NOT the maximal white, but rather how to scale.
///   commonly 1.0, some systems like L*a*b* use other values (100)
/// - luminance range (Lb (luminance black) to Lw (luminance white) for
///   transfer curves like BT.1886)
/// - gamma value for tunable transfer curves
/// - what non-linear transfer curve is employed to encode linear
///   light (or not) into an integer representation
/// - normalization range of values in integer space
///   (the often ignored legal vs. full range vs. whatever)
/// - current color space. A particular volume can be transformed
///   using any of a number of pre-defined transforms into alternate
///   representations of the same data. Examples include RGB, XYZ,
///   YCbCr(Rec.709 vs. Rec.2020 vs. Native), HSL, HSV, et al.
class state
{
public:
	typedef double value_type;
	typedef chromaticities<value_type> cx;

	state( void ) = default;
	constexpr state( space s,
					 const cx &c,
					 value_type lum,
					 range r,
					 transfer crv,
					 value_type crv_a = value_type(1.0),
					 value_type crv_b = value_type(0.0),
					 value_type crv_c = value_type(1.0),
					 value_type crv_d = value_type(0.0),
					 int bits = 32,
					 value_type black_off = value_type(0.0) )
		: _space( s ), _chroma( c ), _lum_scale( lum ),
		  _black_offset( black_off ), _range( r ), _curve( crv ),
		  _curve_a( crv_a ), _curve_b( crv_b ), _curve_c( crv_c ), _curve_d( crv_d ), _bits( bits )
	{}
	state( const state &s ) = default;
	PROPER_CONSTEXPR state( state &&s ) noexcept(std::is_nothrow_move_constructible<value_type>::value) = default;
	state &operator=( const state &s ) = default;
	PROPER_CONSTEXPR state &operator=( state &&s ) noexcept(std::is_nothrow_move_constructible<value_type>::value) = default;
	/// @defgroup convenience constructors to override individual settings
	/// @{
	constexpr state( const state &st, space s )
		: _space( s ), _chroma( st._chroma ), _lum_scale( st._lum_scale ),
		  _black_offset( st._black_offset ), _range( st._range ), _curve( st._curve ),
		  _curve_a( st._curve_a ), _curve_b( st._curve_b ), _curve_c( st._curve_c ),
		  _curve_d( st._curve_d ), _bits( st._bits )
	{}
	constexpr state( const state &st, range r )
		: _space( st._space ), _chroma( st._chroma ), _lum_scale( st._lum_scale ),
		  _black_offset( st._black_offset ), _range( r ), _curve( st._curve ),
		  _curve_a( st._curve_a ), _curve_b( st._curve_b ), _curve_c( st._curve_c ),
		  _curve_d( st._curve_d ), _bits( st._bits )
	{}
	constexpr state( const state &st, transfer t,
					 value_type crv_a = value_type(1.0),
					 value_type crv_b = value_type(0.0),
					 value_type crv_c = value_type(1.0),
					 value_type crv_d = value_type(0.0) )
		: _space( st._space ), _chroma( st._chroma ), _lum_scale( st._lum_scale ),
		  _black_offset( st._black_offset ), _range( st._range ), _curve( t ),
		  _curve_a( crv_a ), _curve_b( crv_b ), _curve_c( crv_c ), _curve_d( crv_d ),
		  _bits( st._bits )
	{}
	/// @}
		
	/// The current space the triplet values corresponding to this state
	/// are transformed to
	constexpr space current_space( void ) const { return _space; }
	/// Set the current space
	inline void current_space( space s ) { _space = s; }

	/// The chromaticities which, when used with the luminance scale
	/// (@sa luminance_scale), can define color transformations
	/// between RGB and XYZ (and on to another RGB)
	constexpr const cx &chroma( void ) const { return _chroma; }
	/// Set the chromaticity coordinates
	inline void chroma( const cx &c ) { _chroma = c; }

	/// This is the scaling of 1.0 to nits (candela / m^2), primarily
	/// for display-referred color states, but useful in general as a
	/// scale for power if desired.
	constexpr value_type luminance_scale( void ) const { return _lum_scale; }
	/// This defines the scaling of 1.0 to nits (candela / m^2).
	/// 
	/// A common value for this is 100, meaning a value of 1.0
	/// corresponds to 100 nits, meaning 18% gray (0.18) is 18 nits
	inline void luminance_scale( value_type s ) { _lum_scale = s; }

	/// The black offset for a display referred image.
	constexpr value_type black_offset( void ) const { return _black_offset; }
	/// Set the black offset value
	///
	/// This can be used as a sort of PLUGE type definition - it
	/// defines the luminance value that corresponds to the minimum
	/// luminance in nits, or candela / m^2, a particular display can
	/// display.
	inline void black_offset( value_type b ) { _black_offset = b; }

	/// return the corresponding scaling of the non-linear data
	constexpr range signal( void ) const { return _range; }
	/// Define any scaling of the non-linear data
	///
	/// Defines the scaling of the data (really only applies to
	/// non-linear data). This is the parameter for the dreaded smpte
	/// / legal vs. full range.
	inline void signal( range r ) { _range = r; }

	/// The transfer curve (OETF or EOTF, depending) that has been
	/// applied to the data corresponding to this state
	constexpr transfer curve( void ) const { return _curve; }
	/// has different meanings depending on the curve
	constexpr value_type curve_a( void ) const { return _curve_a; }
	/// has different meanings depending on the curve
	constexpr value_type curve_b( void ) const { return _curve_b; }
	/// has different meanings depending on the curve
	constexpr value_type curve_c( void ) const { return _curve_c; }
	/// has different meanings depending on the curve
	constexpr value_type curve_d( void ) const { return _curve_d; }

	/// Define the transfer curve applied to the data
	inline void curve( transfer crv,
					   value_type a = value_type(1.0),
					   value_type b = value_type(0.0),
					   value_type c = value_type(1.0),
					   value_type d = value_type(0.0) )
	{
		_curve = crv;
		_curve_a = a;
		_curve_b = b;
		_curve_c = c;
		_curve_d = d;
	}

	/// encoding bits
	constexpr int bits( void ) const { return _bits; }
	/// Defines the bits used in encoding
	///
	/// This is used to adjust the precision used in some of the tone
	/// curve computations as well as the range conversions.
	inline void bits( int b ) { _bits = b; }

private:
	space _space = space::RGB;
	cx _chroma;
	value_type _lum_scale = value_type(100.0);
	value_type _black_offset = value_type(0.0);
	range _range = range::FULL;
	transfer _curve = transfer::LINEAR;
	value_type _curve_a = value_type(1.0);
	value_type _curve_b = value_type(0.0);
	value_type _curve_c = value_type(1.0);
	value_type _curve_d = value_type(0.0);
	int _bits = 10;
};

} // namespace color



