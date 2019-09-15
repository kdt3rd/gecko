// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

////////////////////////////////////////

namespace detail
{
template <standard s> struct standard_ctor
{
    static constexpr standard_definition make( void )
    {
        return ( false ) ? standard_definition()
                         : throw std::invalid_argument( "unknown standard" );
    }
};

template <> struct standard_ctor<standard::SRGB>
{
    static constexpr standard_definition make( void )
    {
        // TODO: determine which is more correct - the standard seems to refer to 0.2 as being
        // the reference black, but the sYCC amendment doc refers to a black of 1, in the
        // flare correction section towards the end of the doc
        return standard_definition(
            two_deg::Rec_709<standard_definition::value_type>(),
            0.2,
            80.0,
            transfer::GAMMA_sRGB,
            transfer::GAMMA_sRGB,
            space::YCBCR_BT709 );
    }
};

template <> struct standard_ctor<standard::BT_601_NTSC>
{
    static constexpr standard_definition make( void )
    {
        return standard_definition(
            two_deg::Rec_601_NTSC<standard_definition::value_type>(),
            0.0,
            100.0,
            transfer::GAMMA_BT601,
            transfer::GAMMA_BT1886,
            space::YCBCR_BT601 );
    }
};

template <> struct standard_ctor<standard::BT_601_PAL>
{
    static constexpr standard_definition make( void )
    {
        return standard_definition(
            two_deg::Rec_601_PAL<standard_definition::value_type>(),
            0.0,
            100.0,
            transfer::GAMMA_BT601,
            transfer::GAMMA_BT1886,
            space::YCBCR_BT601 );
    }
};

template <> struct standard_ctor<standard::BT_709>
{
    static constexpr standard_definition make( void )
    {
        return standard_definition(
            two_deg::Rec_709<standard_definition::value_type>(),
            0.0,
            100.0,
            transfer::GAMMA_BT709,
            transfer::GAMMA_BT1886,
            space::YCBCR_BT709 );
    }
};

template <> struct standard_ctor<standard::BT_2020>
{
    static constexpr standard_definition make( void )
    {
        return standard_definition(
            two_deg::Rec_2020<standard_definition::value_type>(),
            0.0,
            100.0,
            transfer::GAMMA_BT2020,
            transfer::GAMMA_BT1886,
            space::YCBCR_BT2020 );
    }
};

template <> struct standard_ctor<standard::BT_1886>
{
    static constexpr standard_definition make( void )
    {
        return standard_definition(
            two_deg::Rec_709<standard_definition::value_type>(),
            0.0,
            100.0,
            transfer::GAMMA_BT1886,
            transfer::GAMMA_BT1886,
            space::YCBCR_BT709 );
    }
};

template <> struct standard_ctor<standard::BT_2100_PQ>
{
    static constexpr standard_definition make( void )
    {
        return standard_definition(
            two_deg::Rec_709<standard_definition::value_type>(),
            0.0,
            10000.0,
            transfer::PQ,
            transfer::PQ,
            space::ICTCP,
            ootf::BT2100_PQ_OOTF );
    }
};

template <> struct standard_ctor<standard::BT_2100_HLG>
{
    static constexpr standard_definition make( void )
    {
        return standard_definition(
            two_deg::Rec_709<standard_definition::value_type>(),
            0.0,
            1000.0,
            transfer::HLG_OETF,
            transfer::HLG_EOTF,
            space::YCBCR_BT2100,
            ootf::BT2100_HLG_OOTF );
    }
};

template <> struct standard_ctor<standard::ACES_v1_0_3>
{
    static constexpr standard_definition make( void )
    {
        return standard_definition(
            two_deg::ACES_AP0<standard_definition::value_type>(),
            0.0,
            65504.0,
            transfer::LINEAR,
            transfer::LINEAR,
            space::RGB,
            ootf::ACES_RRT_v1_0_3 );
    }
};

template <> struct standard_ctor<standard::DCI_P3>
{
    static constexpr standard_definition make( void )
    {
        return standard_definition(
            two_deg::P3_DCI<standard_definition::value_type>(),
            0.0,
            48.0,
            transfer::GAMMA_DCI,
            transfer::GAMMA_DCI,
            space::XYZ );
    }
};

template <> struct standard_ctor<standard::DCI_P3_D65>
{
    static constexpr standard_definition make( void )
    {
        return standard_definition(
            two_deg::P3_D65<standard_definition::value_type>(),
            0.0,
            48.0,
            transfer::GAMMA_DCI,
            transfer::GAMMA_DCI,
            space::XYZ );
    }
};

template <> struct standard_ctor<standard::SONY_SLOG>
{
    static constexpr standard_definition make( void )
    {
        return standard_definition(
            two_deg::SONY_SGamut<standard_definition::value_type>(),
            0.0,
            100.0,
            transfer::SONY_SLOG3,
            transfer::SONY_SLOG3,
            space::RGB );
    }
};

template <> struct standard_ctor<standard::SONY_SLOG_CINE>
{
    static constexpr standard_definition make( void )
    {
        return standard_definition(
            two_deg::SONY_SGamut3Cine<standard_definition::value_type>(),
            0.0,
            48.0,
            transfer::SONY_SLOG3,
            transfer::SONY_SLOG3,
            space::RGB );
    }
};

template <> struct standard_ctor<standard::ARRI_WIDE_GAMUT>
{
    static constexpr standard_definition make( void )
    {
        return standard_definition(
            two_deg::ARRI_Alexa_WCG<standard_definition::value_type>(),
            0.0,
            48.0,
            transfer::ARRI_LOGC_SCENELIN_SUP3,
            transfer::ARRI_LOGC_SCENELIN_SUP3,
            space::RGB );
    }
};

} // namespace detail
