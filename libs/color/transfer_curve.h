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

/// describes the (non-)linear curve used to encode values used for
/// storing as integer.
enum class transfer
{
	LINEAR, ///< Linear (NO) curve
	GAMMA_sRGB, ///< sRGB EOTF
	GAMMA_BT709, ///< BT.709 OETF
	GAMMA_BT2020, ///< BT.2020 OETF
	GAMMA_BT1886, ///< computes a, b using Lb, Lw from BT.1886 EOTF
	GAMMA_DCI, ///< gamma EOTF for digital cinema, gamma = 2.6
	GAMMA_CUSTOM, ///< custom gamma EOTF, no linear breakpoint
	FOUR_PT_GAMMA_CUSTOM, ///< custom gamma EOTF, linear breakpoint
						  ///< specified by offset, gamma, transition
						  ///< point, slope
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
	MID_GRAY_LOG, ///< mid gray based log designed for OpenEXR
	PQ, ///< ST.2084 Perceptual Quantizer (PQ)
	HLG_OETF, ///< Hybrid Log Gamma OETF per BT.2100
	HLG_EOTF, ///< Hybrid Log Gamma EOTF per BT.2100
	ACES_cc, ///< ACES working space
	ACES_cct, ///< ACES working space2
	ACES_proxy, ///< ACES proxy space
};

} // namespace color



