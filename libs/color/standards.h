//
// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT
// Copyrights licenced under the MIT License.
//

#pragma once

#include "state.h"
#include "illuminant.h"
#include "primaries.h"
#include <stdexcept>

////////////////////////////////////////

namespace color
{

enum class standard
{
	SRGB,
	BT_601_NTSC, // aka CCIR 601, 525 line variant
	BT_601_PAL, // aka CCIR 601, 625 line variant
	BT_709,
	BT_2020,
	BT_1886,
	BT_2100_PQ,
	BT_2100_HLG,
	ACES_v1_0_3, // TODO: do we need separate versioning like this, or can we have metadata?

	// the rest are not standards persay, but common practice
	DCI_P3,
	DCI_P3_D65,
	SONY_SLOG,
	SONY_SLOG_CINE,
	ARRI_WIDE_GAMUT
};

/// @brief Contains values necessary to define the behavior of a color
/// standard.
///
/// Contains the values that are used to define a particular color
/// standard. This is NOT the same as the current state of the pixels,
/// but can be used to transform pixel values from one state to
/// another
class standard_definition
{
public:
	typedef double value_type;
	typedef chromaticities<value_type> cx;

	constexpr standard_definition( void ) = default;
	constexpr standard_definition( const standard_definition & ) noexcept = default;
	standard_definition &operator=( const standard_definition & ) noexcept = default;
	constexpr standard_definition( standard_definition && ) noexcept = default;
	standard_definition &operator=( standard_definition && ) noexcept = default;
	~standard_definition( void ) = default;

	constexpr standard_definition( const cx &c,
								   value_type black,
								   value_type luminance,
								   transfer oetf_crv,
								   transfer eotf_crv,
								   space chromaTransform,
								   ootf renderingFunc = ootf::NOOP )
		: _chroma( c ), _black_lum( black ), _peak_lum( luminance ),
		  _oetf_curve( oetf_crv ), _eotf_curve( eotf_crv ),
		  _chroma_space( chromaTransform ), _rendering( renderingFunc )
	{}

	/// chromaticity coordinates describing the XYZ volume
	/// representing the "normal" color gamut of the standard
	constexpr const cx &chroma( void ) const { return _chroma; }
	/// peak luminance. More generally, mostly for non-integer standards,
	/// corresponds to what 1.0 should map to.
	/// this is the expected peak luminance of the reference
	constexpr value_type peak_luminance( void ) const { return _peak_lum; }
	/// black luminance
	/// this is the expected luminance for black values of the reference
	constexpr value_type black_luminance( void ) const { return _black_lum; }
	/// non-linear OETF (opto-electrical) transfer curve to be used
	constexpr transfer oetf_curve( void ) const { return _oetf_curve; }
	/// non-linear EOTF (electro-optical) transfer curve to be used
	constexpr transfer eotf_curve( void ) const { return _eotf_curve; }
	/// prescribed or preferred color opponent / chroma space
	constexpr space chroma_space( void ) const { return _chroma_space; }
	/// prescribed rendering (mostly only for HDR standards)
	constexpr ootf rendering_ootf( void ) const { return _rendering; }

	// TODO: do we want the actual rounded RGB<->XYZ matrices?
	//       or compute the (more accurate) versions from the chromaticities?
	// TODO: add ambient / surround for viewing environment requirements?

	constexpr state capture_state( range r = range::FULL, bool isRGB = true ) const
	{
		return state( isRGB ? space::RGB : chroma_space(), _chroma, _peak_lum, state::value_type(0), r, _oetf_curve, true );
	}
	constexpr state display_state( range r = range::FULL, bool isRGB = true ) const
	{
		return state( isRGB ? space::RGB : chroma_space(), _chroma, _peak_lum, state::value_type(0), r, _eotf_curve, false );
	}
private:
	cx _chroma;
	value_type _black_lum = value_type(0.0);
	value_type _peak_lum = value_type(100.0);
	transfer _oetf_curve = transfer::LINEAR;
	transfer _eotf_curve = transfer::LINEAR;
	space _chroma_space = space::RGB;
	ootf _rendering = ootf::NOOP;
};

#include "standards_util.h"

template <standard s>
constexpr standard_definition make_standard( void )
{
	using maker = typename detail::standard_ctor<s>;
	return maker::make();
}

constexpr standard_definition make_standard( standard s )
{
	return ( s == standard::SRGB ? make_standard<standard::SRGB>() :
			 ( s == standard::BT_601_NTSC ? make_standard<standard::BT_601_NTSC>() :
			   ( s == standard::BT_601_PAL ? make_standard<standard::BT_601_PAL>() :
				 ( s == standard::BT_709 ? make_standard<standard::BT_709>() :
				   ( s == standard::BT_2020 ? make_standard<standard::BT_2020>() :
					 ( s == standard::BT_1886 ? make_standard<standard::BT_1886>() :
					   ( s == standard::BT_2100_PQ ? make_standard<standard::BT_2100_PQ>() :
						 ( s == standard::BT_2100_HLG ? make_standard<standard::BT_2100_HLG>() :
						   ( s == standard::ACES_v1_0_3 ? make_standard<standard::ACES_v1_0_3>() :
							 ( s == standard::DCI_P3 ? make_standard<standard::DCI_P3>() :
							   ( s == standard::DCI_P3_D65 ? make_standard<standard::DCI_P3_D65>() :
								 ( s == standard::SONY_SLOG ? make_standard<standard::SONY_SLOG>() :
								   ( s == standard::SONY_SLOG_CINE ? make_standard<standard::SONY_SLOG_CINE>() :
									 ( s == standard::ARRI_WIDE_GAMUT ? make_standard<standard::ARRI_WIDE_GAMUT>() :
									   throw std::invalid_argument( "unknown color standard enum" )
									   ) ) ) ) ) ) ) ) ) ) ) ) ) );
}

} // namespace color



