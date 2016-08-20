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

////////////////////////////////////////

namespace color
{

/// Describes the space the pixels are in (i.e. transform away from RGB to something else)
enum class space
{
	RGB, ///< RGB, where chromaticities describe how to convert to XYZ
	XYZ, ///< CIE XYZ, chromaticies record what RGB the XYZ came from
	LMS_CAM02, ///< LMS space, as defined in CIECAM02, chromaticies record originating RGB
	LMS_ICTCP, ///< LMS space, as defined in BT.2100, chromaticies record originating RGB
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
	ICTCP, ///< ICtCp, as defined in BT.2100
	IPT, ///< Ebner & Fairchild, 1998
	HSV_HEX, ///< HSV, using hexagonal approximation
	HSV_CYL, ///< HSV, using cylindrical (polar) math
	HSI_HEX, ///< HSI, using hexagonal approximation
	HSI_CYL, ///< HSI, using cylindrical (polar) math
	HSL_HEX, ///< HSL, using hexagonal approximation
	HSL_CYL, ///< HSL, using cylindrical (polar) math
	/// @}
};

} // namespace color



