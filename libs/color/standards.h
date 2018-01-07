//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
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
	ACES_v1_0_3,

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
	constexpr standard_definition( const cx &c,
								   value_type luminance,
								   transfer oetf_crv,
								   transfer eotf_crv,
								   space chromaTransform,
								   ootf renderingFunc = ootf::NOOP )
		: _chroma( c ), _lum_scale( lum ),
		  _oetf_curve( oetf_crv ), _eotf_curve( eotf_crv ),
		  _chroma_space( chromaTransform ), _rendering( renderingFunc )
	{}

	/// chromaticity coordinates describing the XYZ volume
	/// representing the "normal" color gamut of the standard
	constexpr const cx &chroma( void ) const { return _chroma; }
	/// peak luminance. More generally, mostly for non-integer standards,
	/// corresponds to what 1.0 should map to.
	constexpr value_type peak_luminance( void ) const { return _lum_scale; }
	/// non-linear OETF (opto-electrical) transfer curve to be used
	constexpr transfer oetf_curve( void ) const { return _oetf_curve; }
	/// non-linear EOTF (electro-optical) transfer curve to be used
	constexpr transfer eotf_curve( void ) const { return _eotf_curve; }
	/// prescribed or preferred color opponent / chroma space
	constexpr space chroma_space( void ) const { return _chroma_space; }
	/// prescribed rendering (mostly only for HDR standards)
	constexpr ootf rendering_ootf( void ) const { return _rendering; }

	// TODO: add ambient / surround for viewing environment?
private:
	cx _chroma;
	value_type _lum_scale = value_type(100.0);
	transfer _oetf_curve = transfer::LINEAR;
	transfer _eotf_curve = transfer::LINEAR;
	space _chroma_space = space::RGB;
	ootf _rendering = ootf::NOOP;
};

namespace detail
{

template <standard s>
struct standard_ctor
{
	static constexpr standard_definition make( void ) { return (false) ? standard_definition() : throw std::invalid_argument( "unknown standard" ); }
};

template <>
struct standard_ctor<standard::SRGB>
{
	static constexpr standard_definition make( void )
	{
		return standard_definition( two_deg::Rec_709<standard_definition::value_type>(),
									80.0, transfer::GAMMA_sRGB, transfer::GAMMA_sRGB,
									space::YCBCR_BT709 );
	}
};

template <>
struct standard_ctor<standard::BT_601_NTSC>
{
	static constexpr standard_definition make( void )
	{
		return standard_definition( two_deg::Rec_601_NTSC<standard_definition::value_type>(),
									100.0, transfer::GAMMA_BT601, transfer::GAMMA_BT1886,
									space::YCBCR_BT601 );
	}
};

template <>
struct standard_ctor<standard::BT_601_PAL>
{
	static constexpr standard_definition make( void )
	{
		return standard_definition( two_deg::Rec_601_PAL<standard_definition::value_type>(),
									100.0, transfer::GAMMA_BT601, transfer::GAMMA_BT1886,
									space::YCBCR_BT601 );
	}
};

template <>
struct standard_ctor<standard::BT_709>
{
	static constexpr standard_definition make( void )
	{
		return standard_definition( two_deg::Rec_709<standard_definition::value_type>(),
									100.0, transfer::GAMMA_BT709, transfer::GAMMA_BT1886,
									space::YCBCR_BT709 );
	}
};

template <>
struct standard_ctor<standard::BT_2020>
{
	static constexpr standard_definition make( void )
	{
		return standard_definition( two_deg::Rec_2020<standard_definition::value_type>(),
									100.0, transfer::GAMMA_BT2020, transfer::GAMMA_BT1886,
									space::YCBCR_BT2020 );
	}
};

template <>
struct standard_ctor<standard::BT_1886>
{
	static constexpr standard_definition make( void )
	{
		return standard_definition( two_deg::Rec_709<standard_definition::value_type>(),
									100.0, transfer::GAMMA_BT1886, transfer::GAMMA_BT1886,
									space::YCBCR_BT709 );
	}
};

template <>
struct standard_ctor<standard::BT_2100_PQ>
{
	static constexpr standard_definition make( void )
	{
		return standard_definition( two_deg::Rec_709<standard_definition::value_type>(),
									100.0, transfer::PQ, transfer::PQ, space::ICTCP,
									ootf::BT2100_PQ_OOTF );
	}
};

template <>
struct standard_ctor<standard::BT_2100_HLG>
{
	static constexpr standard_definition make( void )
	{
		return standard_definition( two_deg::Rec_709<standard_definition::value_type>(),
									100.0, transfer::HLG_OETF, transfer::HLG_EOTF,
									space::YCBCR_BT2100, ootf::BT2100_HLG_OOTF );
	}
};

template <>
struct standard_ctor<standard::ACES_v1_0_3>
{
	static constexpr standard_definition make( void )
	{
		return standard_definition( two_deg::ACES_AP0<standard_definition::value_type>(),
									100.0, transfer::LINEAR, transfer::LINEAR,
									space::RGB, ootf::ACES_RRT_v1_0_3 );
	}
};

template <>
struct standard_ctor<standard::DCI_P3>
{
	static constexpr standard_definition make( void )
	{
		return standard_definition( two_deg::P3_DCI<standard_definition::value_type>(),
									48.0, transfer::GAMMA_DCI, transfer::GAMMA_DCI,
									space::XYZ );
	}
};

template <>
struct standard_ctor<standard::DCI_P3_D65>
{
	static constexpr standard_definition make( void )
	{
		return standard_definition( two_deg::P3_D65<standard_definition::value_type>(),
									48.0, transfer::GAMMA_DCI, transfer::GAMMA_DCI,
									space::XYZ );
	}
};

template <>
struct standard_ctor<standard::SONY_SLOG>
{
	static constexpr standard_definition make( void )
	{
		return standard_definition( two_deg::SONY_SGamut<standard_definition::value_type>(),
									100.0, transfer::SONY_SLOG3, transfer::SONY_SLOG3,
									space::RGB );
	}
};

template <>
struct standard_ctor<standard::SONY_SLOG_CINE>
{
	static constexpr standard_definition make( void )
	{
		return standard_definition( two_deg::SONY_SGamut3Cine<standard_definition::value_type>(),
									48.0, transfer::SONY_SLOG3, transfer::SONY_SLOG3,
									space::RGB );
	}
};

template <>
struct standard_ctor<standard::ARRI_WIDE_GAMUT>
{
	static constexpr standard_definition make( void )
	{
		return standard_definition( two_deg::ARRI_Alexa_WCG<standard_definition::value_type>(),
									48.0,
									transfer::ARRI_LOGC_SCENELIN_SUP3,
									transfer::ARRI_LOGC_SCENELIN_SUP3,
									space::RGB );
	}
};

} // namespace detail

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



