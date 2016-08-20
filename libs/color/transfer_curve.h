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



