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


////////////////////////////////////////


namespace color
{

/// @brief Describes the normalization of encoded values
///
/// This covers all the variants of the classic full vs. legal 
enum class range
{
	FULL, ///< normal 0 - 2^bits - 1 integer encoding 0.0 - 1.0
	ITU_FULL, ///< 0 - 2^bits ITU-defined scaling
	LEGAL,  ///< for luma: 16-235 (scaled based on bits)
			///< for chroma: 16-240 (scaled based on bits)
			///< reserved values: 0-255 for 8 bits, 4-1019 for 10, etc.
	SIGNAL, ///< same reserved values as LEGAL, but
			///< scales the non-reserved values 0-1 with
			///< no extra gap range
	LEGAL_EXTENDED  ///< sometimes called SMPTE+, this is
					///< a combination of LEGAL and SIGNAL
					///< where the reference black is the same
					///< as legal, but the reference white is
					///< the same as signal
};

enum class transfer
{
	LINEAR, ///< Linear (NO) curve
	GAMMA_sRGB, ///< sRGB EOTF
	GAMMA_BT709, ///< BT.709 OETF
	GAMMA_BT2020, ///< BT.2020 OETF
	GAMMA_BT1886, ///< computes a, b using Lb, Lw from BT.1886 EOTF
	SONY_SLOG1, ///< Sony S-Log v1
	SONY_SLOG2, ///< Sony S-Log v2
	SONY_SLOG3, ///< Sony S-Log v3
	ARRI_LOGC, ///< ARRI Log-C (need to know EI and version to know
			   ///< parameters for function)
	CINEON, ///< Cineon printing density
	ST_2084, ///< ST.2084 Perceptual Quantizer (PQ)
	BBC_HLG, ///< BBC Hybrid Log Gamma
};

/// @brief Class description provides a basic description of attributes
///        describing a color.
///
/// This is most commonly used to describe how to interpret color channels
/// of an image, and then can be used to compute the transformation between
/// different representations.
///
/// There are a few basic attributes that determine this:
/// - the underlying chromaticities of the volume involved
///   (describing the RGB<->XYZ tranformation)
/// - diffuse / reference white scale value.
///   NB: This is NOT the maximal white, but rather how to scale.
///   commonly 1.0, some systems like L*a*b* use other values (100)
/// - luminance range (Lb (luminance black) to Lw (luminance white) for
///   transfer curves like BT.1886)
/// - what non-linear transfer curve is employed to encode linear
///   light (or not) into an integer representation
/// - normalization range of values in integer space
///   (the often ignored legal vs. full range vs. whatever)
/// - current color space. A particular volume can be transformed
///   using any of a number of pre-defined transforms into alternate
///   representations of the same data. Examples include RGB, XYZ,
///   YCbCr(Rec.709 vs. Rec.2020 vs. Native), HSL, HSV, et al.
class description
{
public:
	typedef double value_type;
	typedef chromaticities<value_type> cx;

	inline const cx &chroma( void ) const { return _chroma; }
	inline void chroma( const cx &c ) { _chroma = c; }

	inline range signal( void ) const { return _range; }
	inline void signal( range r ) { _range = r; }

	inline transfer curve( void ) const { return _curve; }
	inline void curve( transfer c ) { _curve = c; }

	inline std::pair<value_type,value_type> luminance( void ) const { return _lum; }
	inline void luminance( value_type black, value_type white ) { _lum = std::make_pair( black, white ); }

	inline value_type reference_scale( void ) { return _ref_scale; }
	inline void reference_scale( value_type s ) { _ref_scale = s; }

private:
	cx _chroma;
	std::pair<value_type, value_type> _lum;
	value_type _ref_scale;
	range _range;
	transfer _curve;
};

} // namespace color



