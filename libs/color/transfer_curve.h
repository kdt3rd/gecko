// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <iostream>

// the following implementations of transfer curves are expected to
// provide a (templated) structure implementing the following:
// 1. static functions for linearize / encode of that transfer curve
// 2. non-static to_linear/from_linear functions if the transfer curve is
//    instantiated
// 3. An interface to get a std::function that linearizes / encodes
// 4. functions to provide {to/from}_linear_glsl that return the string text
//    of a glsl function implementing the appropriate transform
// 5. any other engine / language generation support needed in the future?
//
// TODO: move all the math to a separate header tree to lower dependencies
// when just carrying the state???
#include "transfer_curves/mid_gray_log.h"
#include "transfer_curves/gamma_srgb.h"
#include "transfer_curves/cie_Lab_76.h"

////////////////////////////////////////

namespace color
{

/// describes the (non-)linear curve used to encode values used for
/// storing as integer.
enum class transfer : uint8_t
{
	LINEAR, ///< Linear (NO) curve
	GAMMA_sRGB, ///< sRGB EOTF
	GAMMA_BT601, ///< BT.601 OETF
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
	CINEON, ///< Old Cineon printing density / display conversion (no softclip)
	CINEON_SOFTCLIP, ///< Cineon printing density with softclip
	MID_GRAY_LOG, ///< mid gray based log from Josh Pines, developed originally for OpenEXR
	PQ, ///< ST.2084 Perceptual Quantizer (PQ) / Dolby PQ
	HLG_OETF, ///< Hybrid Log Gamma OETF per BT.2100
	HLG_EOTF, ///< Hybrid Log Gamma EOTF per BT.2100
	ACES_cc, ///< ACES working space
	ACES_cct, ///< ACES working space2
	ACES_proxy, ///< ACES proxy space
	CIE_LAB_76, ///< CIE L*a*b* non-linear curve from 1976
};

/// @brief standard rendering functions (opto-opto transforms)
enum class ootf : uint8_t
{
	NOOP, ///< nothing
	BT709_RENDERING, ///< as recommended, bt.709 is supposed to be
					 ///< viewed through bt.1886, which applies a
					 ///< rendering function to bt.709
	BT2100_PQ_OOTF = BT709_RENDERING, ///< PQ OOTF
	BT2100_HLG_OOTF, ///< OOTF defined for HLG
	ACES_RRT_v1_0_3, ///< ACES RRT v1.0.3
};

struct transfer_curve_control
{
	typedef double value_type;

	constexpr transfer_curve_control( void ) noexcept = default;
	constexpr transfer_curve_control( value_type a, value_type b,
									  value_type c, value_type d,
									  value_type e ) noexcept
		: _A( a ), _B( b ), _C( c ), _D( d ), _E( e )
	{}
	constexpr transfer_curve_control( const transfer_curve_control &s ) noexcept = default;
	constexpr transfer_curve_control( transfer_curve_control &&s ) noexcept = default;
	transfer_curve_control &operator=( const transfer_curve_control &s ) noexcept = default;
	transfer_curve_control &operator=( transfer_curve_control &&s ) noexcept = default;
	~transfer_curve_control( void ) = default;

	value_type _A = value_type(1);
	value_type _B = value_type(0);
	value_type _C = value_type(1);
	value_type _D = value_type(0);
	value_type _E = value_type(0);
};

inline constexpr transfer_curve_control
get_curve_defaults( transfer t )
{
	// four pt gamma has values too, but the defaults are good per above
	// TODO: know about the ARRI E.I.
	return t == transfer::MID_GRAY_LOG ?
		transfer_curve_control( 0.18, 445, 0.6, 0.002, 1023 ) :
		transfer_curve_control();
}

//inline std::vector<std::string>
//get_curve_parameter_names( transfer t )
//{
//}

template <typename T>
inline T linearize( const T v, transfer t )
{
	switch ( t )
	{
		case transfer::LINEAR: return v;
		case transfer::GAMMA_sRGB: return gamma_srgb<T>::linearize( v );
		case transfer::GAMMA_BT601: return gamma_srgb<T>::linearize( v );
		case transfer::GAMMA_BT709: return gamma_srgb<T>::linearize( v );
		case transfer::GAMMA_BT2020: return gamma_srgb<T>::linearize( v );
		case transfer::GAMMA_BT1886: return gamma_srgb<T>::linearize( v );
		case transfer::GAMMA_DCI: return gamma_srgb<T>::linearize( v );
		case transfer::GAMMA_CUSTOM: return gamma_srgb<T>::linearize( v );
		case transfer::FOUR_PT_GAMMA_CUSTOM: return gamma_srgb<T>::linearize( v );
		case transfer::SONY_SLOG1: return gamma_srgb<T>::linearize( v );
		case transfer::SONY_SLOG2: return gamma_srgb<T>::linearize( v );
		case transfer::SONY_SLOG3: return gamma_srgb<T>::linearize( v );
		case transfer::ARRI_LOGC_NORMSENS_SUP2: return gamma_srgb<T>::linearize( v );
		case transfer::ARRI_LOGC_SCENELIN_SUP2: return gamma_srgb<T>::linearize( v );
		case transfer::ARRI_LOGC_NORMSENS_SUP3: return gamma_srgb<T>::linearize( v );
		case transfer::ARRI_LOGC_SCENELIN_SUP3: return gamma_srgb<T>::linearize( v );
		case transfer::CINEON: return gamma_srgb<T>::linearize( v );
		case transfer::CINEON_SOFTCLIP: return gamma_srgb<T>::linearize( v );
		case transfer::MID_GRAY_LOG: return mid_gray_log<T>::linearize( v );
		case transfer::PQ: return gamma_srgb<T>::linearize( v );
		case transfer::HLG_OETF: return gamma_srgb<T>::linearize( v );
		case transfer::HLG_EOTF: return gamma_srgb<T>::linearize( v );
		case transfer::ACES_cc: return gamma_srgb<T>::linearize( v );
		case transfer::ACES_cct: return gamma_srgb<T>::linearize( v );
		case transfer::ACES_proxy: return gamma_srgb<T>::linearize( v );
		case transfer::CIE_LAB_76: return cie_Lab_76<T>::linearize( v );
	}
	return v;
}

template <typename T>
inline T encode( const T v, transfer t )
{
	switch ( t )
	{
		case transfer::LINEAR: return v;
		case transfer::GAMMA_sRGB: return gamma_srgb<T>::encode( v );
		case transfer::GAMMA_BT601: return gamma_srgb<T>::encode( v );
		case transfer::GAMMA_BT709: return gamma_srgb<T>::encode( v );
		case transfer::GAMMA_BT2020: return gamma_srgb<T>::encode( v );
		case transfer::GAMMA_BT1886: return gamma_srgb<T>::encode( v );
		case transfer::GAMMA_DCI: return gamma_srgb<T>::encode( v );
		case transfer::GAMMA_CUSTOM: return gamma_srgb<T>::encode( v );
		case transfer::FOUR_PT_GAMMA_CUSTOM: return gamma_srgb<T>::encode( v );
		case transfer::SONY_SLOG1: return gamma_srgb<T>::encode( v );
		case transfer::SONY_SLOG2: return gamma_srgb<T>::encode( v );
		case transfer::SONY_SLOG3: return gamma_srgb<T>::encode( v );
		case transfer::ARRI_LOGC_NORMSENS_SUP2: return gamma_srgb<T>::encode( v );
		case transfer::ARRI_LOGC_SCENELIN_SUP2: return gamma_srgb<T>::encode( v );
		case transfer::ARRI_LOGC_NORMSENS_SUP3: return gamma_srgb<T>::encode( v );
		case transfer::ARRI_LOGC_SCENELIN_SUP3: return gamma_srgb<T>::encode( v );
		case transfer::CINEON: return gamma_srgb<T>::encode( v );
		case transfer::CINEON_SOFTCLIP: return gamma_srgb<T>::encode( v );
		case transfer::MID_GRAY_LOG: return mid_gray_log<T>::encode( v );
		case transfer::PQ: return gamma_srgb<T>::encode( v );
		case transfer::HLG_OETF: return gamma_srgb<T>::encode( v );
		case transfer::HLG_EOTF: return gamma_srgb<T>::encode( v );
		case transfer::ACES_cc: return gamma_srgb<T>::encode( v );
		case transfer::ACES_cct: return gamma_srgb<T>::encode( v );
		case transfer::ACES_proxy: return gamma_srgb<T>::encode( v );
		case transfer::CIE_LAB_76: return cie_Lab_76<T>::encode( v );
	}
	return v;
}

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &os, const transfer &t )
{
	switch ( t )
	{
		case transfer::LINEAR: os << "linear"; break;
		case transfer::GAMMA_sRGB: os << "gamma_srgb"; break;
		case transfer::GAMMA_BT601: os << "gamma_bt601"; break;
		case transfer::GAMMA_BT709: os << "gamma_bt709"; break;
		case transfer::GAMMA_BT2020: os << "gamma_bt2020"; break;
		case transfer::GAMMA_BT1886: os << "gamma_bt1886"; break;
		case transfer::GAMMA_DCI: os << "gamma_dci"; break;
		case transfer::GAMMA_CUSTOM: os << "gamma_custom"; break;
		case transfer::FOUR_PT_GAMMA_CUSTOM: os << "fourpt_gamma"; break;
		case transfer::SONY_SLOG1: os << "slog1"; break;
		case transfer::SONY_SLOG2: os << "slog2"; break;
		case transfer::SONY_SLOG3: os << "slog3"; break;
		case transfer::ARRI_LOGC_NORMSENS_SUP2: os << "arri_logc_sens2"; break;
		case transfer::ARRI_LOGC_SCENELIN_SUP2: os << "arri_logc_scene2"; break;
		case transfer::ARRI_LOGC_NORMSENS_SUP3: os << "arri_logc_sens3"; break;
		case transfer::ARRI_LOGC_SCENELIN_SUP3: os << "arri_logc_scene3"; break;
		case transfer::CINEON: os << "cineon"; break;
		case transfer::CINEON_SOFTCLIP: os << "cineon_soft"; break;
		case transfer::MID_GRAY_LOG: os << "midgraylog"; break;
		case transfer::PQ: os << "pq"; break;
		case transfer::HLG_OETF: os << "hlg_oetf"; break;
		case transfer::HLG_EOTF: os << "hlg_eotf"; break;
		case transfer::ACES_cc: os << "aces_cc"; break;
		case transfer::ACES_cct: os << "aces_cct"; break;
		case transfer::ACES_proxy: os << "aces_proxy"; break;
		case transfer::CIE_LAB_76: os << "cielab76"; break;
	}
	return os;
}

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &os, const transfer_curve_control &tcc )
{
	os << tcc._A << ',' << tcc._B << ',' << tcc._C << ',' << tcc._D << ',' << tcc._E;
	return os;
}

} // namespace color




