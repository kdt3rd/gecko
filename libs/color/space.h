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
/// The components in a tri-stimulus value be in the order that the acronym is in, so
/// XYZ will be X = v.x(), Y = v.y(), Z = v.z(), RGB will be R = v.x(), G = v.y(), B = v.z(),
/// YCbCr will be Y = v.x(), Cb = v.y(), Cr = v.z(), and so on
enum class space : uint8_t
{
	RGB, ///< RGB, where chromaticities describe how to convert to XYZ
	XYZ, ///< CIE XYZ, chromaticies record what RGB the XYZ came from
	xyY, ///< CIE xyY projection, chromaticies record what RGB the XYZ came from
	LMS_HPE, ///< LMS space, as defined for Hunt / RLAB based on Hunt-Pointer-Estevez
	LMS_CAM02, ///< LMS space, as defined in CIECAM02, chromaticies record originating RGB
	LMS_ICTCP, ///< LMS space, as defined in BT.2100, chromaticies record originating RGB
	CHONG, ///< perceptually uniform, illuminant invariant, per Chong et al. siggraph 2008

	/// @defgroup Color opponent spaces attempting to separate
	/// intensity / luminance / brightness from chroma / saturation / hue
	/// @{
	CIE_LAB_76, ///< CIE L*a*b* 1976, chromaticies record what RGB it
				///< came from, white point for normalization
	HUNTER_LAB, ///< Hunter Lab, chromaticies record what RGB it came from,
				///< white point for normalization
	CIE_LUV_76, ///< CIE L*u*v*, from CIE 1976
	CIE_LCH_76, ///< CIE LCh, similar to Lab, but cylindrical coordinates
				///< for chroma (relative saturation) and hue
	CIE_UVW_64, ///< CIE U*V*W*, from CIE 1964
	LMS_OPPONENT, ///< LMS-based color opponent space per Wandell, Foundations of Vision
	YCBCR_BT601, ///< YCbCr as defined in BT.601
	YCBCR_BT709, ///< YCbCr as defined in BT.709
	YCBCR_BT2020, ///< YCbCr as defined in BT.2020
	YCBCR_BT2100, ///< YCbCr as defined in BT.2100
	YCBCR_SYCC, ///< YCbCr as defined in amendment 1 to IEC 61966-2-1:1999 (sRGB)
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
	UNKNOWN ///< in some unknown state
};

// ycbcr_sYCC
//
// [ Y  ]   [  0.2990  0.5870  0.1140 ][ R ]
// [ Cb ] = [ -0.1687 -0.3312  0.5000 ][ G ]
// [ Cr ]   [  0.5000 -0.4187 -0.0813 ][ B ]

// wandell LMS_OPPONENT
//
// [ O1 ] = [  1.0000  0.0000  0.0000 ][ L ]
// [ O2 ] = [ -0.5900  0.8000 -0.1200 ][ M ]
// [ O3 ] = [ -0.3400 -0.1100  0.9300 ][ S ]

} // namespace color



