//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
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



