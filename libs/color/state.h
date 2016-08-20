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

	/// The chromaticities which, when used with the luminance scale
	/// (@sa luminance_scale), can define color transformations
	/// between RGB and XYZ (and on to another RGB)
	inline const cx &chroma( void ) const { return _chroma; }
	/// Set the chromaticity coordinates
	inline void chroma( const cx &c ) { _chroma = c; }

	/// This is the scaling of 1.0 to nits (candela / m^2)
	inline value_type luminance_scale( void ) const { return _lum_scale; }
	/// This defines the scaling of 1.0 to nits (candela / m^2).
	/// 
	/// A common value for this is 100, meaning a value of 1.0
	/// corresponds to 100 nits, meaning 18% gray (0.18) is 18 nits
	inline void luminance_scale( value_type s ) { _lum_scale = s; }

	/// The black offset for a display referred image.
	inline value_type black_offset( void ) const { return _black_offset; }
	/// Set the black offset value
	///
	/// This can be used as a sort of PLUGE type definition - it
	/// defines the luminance value that corresponds to the minimum
	/// luminance in nits, or candela / m^2, a particular display can
	/// display.
	inline void black_offset( value_type b ) { _black_offset = b; }

	/// return the corresponding scaling of the non-linear data
	inline range signal( void ) const { return _range; }
	/// Define any scaling of the non-linear data
	///
	/// Defines the scaling of the data (really only applies to
	/// non-linear data). This is the parameter for the dreaded smpte
	/// / legal vs. full range.
	inline void signal( range r ) { _range = r; }

	/// The transfer curve (OETF or EOTF, depending) that has been
	/// applied to the data corresponding to this state
	inline transfer curve( void ) const { return _curve; }
	/// has different meanings depending on the curve
	inline value_type curve_a( void ) const { return _curve_a; }
	/// has different meanings depending on the curve
	inline value_type curve_b( void ) const { return _curve_b; }
	/// has different meanings depending on the curve
	inline value_type curve_c( void ) const { return _curve_c; }
	/// has different meanings depending on the curve
	inline value_type curve_d( void ) const { return _curve_d; }

	/// Define the transfer curve applied to the data
	inline void curve( transfer crv,
					   value_type a,
					   value_type b,
					   value_type c,
					   value_type d )
	{
		_curve = crv;
		_curve_a = a;
		_curve_b = b;
		_curve_c = c;
		_curve_d = d;
	}

	/// encoding bits
	inline int bits( void ) const { return _bits; }
	/// Defines the bits used in encoding
	///
	/// This is used to adjust the precision used in some of the tone
	/// curve computations as well as the range conversions.
	inline void bits( int b ) { _bits = b; }

private:
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



