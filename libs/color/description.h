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
	ITU_FULL, ///< 0 - 2^bits ITU-BT.HDR defined scaling
	SMPTE,  ///< also called legal or video range
			///< for luma: 16-235 (scaled based on bits)
			///< for chroma: 16-240 (scaled based on bits)
			///< reserved values: 0,255 for 8 bits, 0-3,1020-1023 for 10, etc.
	SIGNAL, ///< same reserved values as SMPTE, but
			///< scales the non-reserved values 0-1 with
			///< no extra gap range
	SMPTE_PLUS  ///< A combination of SMPTE and SIGNAL
		///< where the black level is the same
		///< as legal, but the peak is the same as signal
};

enum class space
{
	RGB, ///< RGB, where chromaticities describe how to convert to XYZ
	XYZ, ///< CIE XYZ, chromaticies record what RGB the XYZ came from
	LMS_CAM02, ///< LMS space, as defined in CIECAM02
	LMS_ICTCP, ///< LMS space, as defined in BT.HDR
	CHONG, ///< perceptually uniform, illuminant invariant, per Chong et al. siggraph 2008

	/// @defgroup Color opponent spaces attempting to separate
	/// intensity / luminance / brightness from chroma / saturation / hue
	/// @{
	LAB, ///< CIE L*a*b*, chromaticies record what RGB it came from,
		 ///< white point for normalization
	HUNTER_LAB, ///< Hunter Lab, chromaticies record what RGB it came from,
				///< white point for normalization
	LCH, ///< CIE LCh, similar to Lab, but cylindrical coordinates for
		 ///< chroma (relative saturation) and hue
	LUV, ///< CIE L*u*v*, from CIE 1976
	YCBCR_BT601, ///< YCbCr as defined in BT.601
	YCBCR_BT709, ///< YCbCr as defined in BT.709
	YCBCR_BT2020, ///< YCbCr as defined in BT.2020
	YCBCR_CUSTOM, ///< YCbCr computed using chromaticities
	ICTCP_BTHDR, ///< ICtCp, as defined in BT.HDR
	IPT, ///< Ebner & Fairchild, 1998
	HSV_HEX, ///< HSV, using hexagonal approximation
	HSV_CYL, ///< HSV, using cylindrical (polar) math
	HSI_HEX, ///< HSI, using hexagonal approximation
	HSI_CYL, ///< HSI, using cylindrical (polar) math
	HSL_HEX, ///< HSL, using hexagonal approximation
	HSL_CYL, ///< HSL, using cylindrical (polar) math
	/// @}
};

enum class transfer
{
	LINEAR, ///< Linear (NO) curve
	GAMMA_sRGB, ///< sRGB EOTF
	GAMMA_BT709, ///< BT.709 OETF
	GAMMA_BT2020, ///< BT.2020 OETF
	GAMMA_BT1886, ///< computes a, b using Lb, Lw from BT.1886 EOTF
	GAMMA_DCI, ///< gamma EOTF for digital cinema, gamma = 2.6
	GAMMA_CUSTOM, ///< custom gamma EOTF, no linear breakpoint
	SONY_SLOG1, ///< Sony S-Log v1
	SONY_SLOG2, ///< Sony S-Log v2
	SONY_SLOG3, ///< Sony S-Log v3 encoding of linear scene reflection
	ARRI_LOGC_NORMSENS_SUP2, ///< ARRI Log-C SUP 2.X or earlier normalized
							 ///< sensor signal (need to know EI)
	ARRI_LOGC_SCENELIN_SUP2, ///< ARRI Log-C SUP 2.X or earlier linear scene
							 ///< exposure (need to know EI)
	ARRI_LOGC_NORMSENS_SUP3, ///< ARRI Log-C SUP 3.X or newer normalized
							 ///< sensor signal (need to know EI)
	ARRI_LOGC_SCENELIN_SUP3, ///< ARRI Log-C SUP 3.X or newer linear scene
							 ///< exposure (need to know EI)
	CINEON, ///< Cineon printing density
	ST_2084, ///< ST.2084 Perceptual Quantizer (PQ)
	BBC_HLG, ///< BBC Hybrid Log Gamma (need to know the system gamma)
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
class description
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
	/// applied to the data corresponding to this description
	inline transfer curve( void ) const { return _curve; }
	inline value_type curve_gamma( void ) const { return _curve_gamma; }
	inline value_type curve_black( void ) const { return _curve_black; }
	inline value_type curve_white( void ) const { return _curve_white; }

	/// Define the transfer curve applied to the data
	inline void curve( transfer c,
					   value_type Lb,
					   value_type Lw,
					   value_type gamma )
	{
		_curve = c;
		_curve_black = Lb;
		_curve_white = Lw;
		_curve_gamma = gamma;
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
	value_type _curve_gamma = value_type(1.0);
	value_type _curve_black = value_type(0.0);
	value_type _curve_white = value_type(1.0);
	int _bits = 10;
};

} // namespace color



