// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "chromaticities.h"
#include "primaries.h"
#include "range.h"
#include "space.h"
#include "transfer_curve.h"

#include <iostream>

////////////////////////////////////////

namespace color
{
/// @brief Class description provides a basic set of attributes
///        describing the current state of an image or other color
///        value.
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
/// - black pedestal / offset
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
class state
{
public:
    typedef double                     value_type;
    typedef chromaticities<value_type> cx;

    constexpr state( void ) noexcept = default;
    constexpr state(
        space      s,
        const cx & c,
        value_type lum,
        value_type black_off,
        range      lumr,
        transfer   crv,
        bool       scenerefer ) noexcept
        : _space( s )
        , _range( lumr )
        , _curve( crv )
        , _chroma( c )
        , _lum_scale( lum )
        , _black_offset( black_off )
        , _scene_referred( scenerefer )
        , _curve_ctl( get_curve_defaults( crv ) )
    {}
    constexpr state( const state &s ) noexcept = default;
    constexpr state( state &&s ) noexcept      = default;
    state &operator=( const state &s ) noexcept = default;
    state &operator=( state &&s ) noexcept = default;
    /// @defgroup convenience constructors to override individual settings
    /// @{
    constexpr state( const state &st, space s )
        : _space( s )
        , _range( st._range )
        , _curve( st._curve )
        , _chroma( st._chroma )
        , _lum_scale( st._lum_scale )
        , _black_offset( st._black_offset )
        , _scene_referred( st._scene_referred )
        , _curve_ctl( st._curve_ctl )
    {}
    constexpr state( const state &st, range r )
        : _space( st._space )
        , _range( r )
        , _curve( st._curve )
        , _chroma( st._chroma )
        , _lum_scale( st._lum_scale )
        , _black_offset( st._black_offset )
        , _scene_referred( st._scene_referred )
        , _curve_ctl( st._curve_ctl )
    {}
    constexpr state( const state &st, transfer t, bool reset = true )
        : _space( st._space )
        , _range( st._range )
        , _curve( t )
        , _chroma( st._chroma )
        , _lum_scale( st._lum_scale )
        , _black_offset( st._black_offset )
        , _scene_referred( st._scene_referred )
        , _curve_ctl( reset ? get_curve_defaults( t ) : st._curve_ctl )
    {}
    constexpr state(
        const state &st, transfer t, const transfer_curve_control &ctl )
        : _space( st._space )
        , _range( st._range )
        , _curve( t )
        , _chroma( st._chroma )
        , _lum_scale( st._lum_scale )
        , _black_offset( st._black_offset )
        , _scene_referred( st._scene_referred )
        , _curve_ctl( ctl )
    {}
    /// @}
    ~state( void ) = default;

    /// The current space the triplet values corresponding to this state
    /// are transformed to
    constexpr space current_space( void ) const { return _space; }
    /// Set the current space
    inline void current_space( space s ) { _space = s; }

    /// The chromaticities which, when used with the luminance scale
    /// (@sa luminance_scale), can define color transformations
    /// between RGB and XYZ (and on to another RGB)
    constexpr const cx &chroma( void ) const { return _chroma; }
    /// Set the chromaticity coordinates
    inline void chroma( const cx &c ) { _chroma = c; }

    /// This is the scaling of 1.0 to nits (candela / m^2), primarily
    /// for display-referred color states, but useful in general as a
    /// scale for power if desired.
    constexpr value_type luminance_scale( void ) const { return _lum_scale; }
    /// This defines the scaling of 1.0 to nits (candela / m^2).
    ///
    /// A common value for this is 100, meaning a value of 1.0
    /// corresponds to 100 nits, meaning 18% gray (0.18) is 18 nits
    inline void luminance_scale( value_type s ) { _lum_scale = s; }

    /// The black offset for a display referred image.
    constexpr value_type black_offset( void ) const { return _black_offset; }
    /// Set the black offset value
    ///
    /// This can be used as a sort of PLUGE type definition - it
    /// defines the luminance value that corresponds to the minimum
    /// luminance in nits, or candela / m^2, a particular display can
    /// display.
    inline void black_offset( value_type b ) { _black_offset = b; }

    /// return the corresponding scaling of the non-linear data
    constexpr range signal( void ) const { return _range; }
    /// Define any scaling of the non-linear data
    ///
    /// Defines the scaling of the data (really only applies to
    /// non-linear data). This is the parameter for the dreaded smpte
    /// / legal vs. full range.
    inline void signal( range r ) { _range = r; }

    /// The transfer curve (OETF or EOTF, depending) that has been
    /// applied to the data corresponding to this state
    constexpr transfer curve( void ) const { return _curve; }
    /// has different meanings depending on the curve
    constexpr const transfer_curve_control &curve_controls( void ) const
    {
        return _curve_ctl;
    }

    /// Define the transfer curve applied to the data
    inline void curve( transfer crv, bool reset_controls = true )
    {
        _curve = crv;
        if ( reset_controls )
            _curve_ctl = get_curve_defaults( crv );
    }
    inline void curve( transfer crv, const transfer_curve_control &ctl )
    {
        _curve     = crv;
        _curve_ctl = ctl;
    }

    /// Indicates if the color state is scene referred, meaning the luminance
    /// scale value is also scene referred.
    ///
    /// By extension, if false, the color state is in a display referred state,
    /// indicating the color has been rendered for a display
    constexpr bool scene_referred( void ) const { return _scene_referred; }
    /// Alternate / opposite way of querying if the color state is
    /// rendered or in a scene referred state
    constexpr bool rendered( void ) const { return !scene_referred(); }
    /// indicate the color has been rendered for a display or not
    inline void rendered( bool r ) { _scene_referred = !r; }

    inline bool is_same_matrix( const state &o ) const
    {
        /// \todo Deal with adaptation matrices
        /// \todo deal with collapsing ycbcr->rgb?
        return _chroma == o._chroma;
    }

    inline cx::mat get_to_xyz_mat( value_type Y = value_type( 1 ) ) const
    {
        switch ( _space )
        {
            case space::RGB:
            case space::YCBCR_BT601:
            case space::YCBCR_BT709:
            case space::YCBCR_BT2020:
            case space::YCBCR_BT2100:
            case space::YCBCR_SYCC:
            case space::YCBCR_CUSTOM:
            case space::HSV_HEX:
            case space::HSV_CYL:
            case space::HSI_HEX:
            case space::HSI_CYL:
            case space::HSL_HEX:
            case space::HSL_CYL: break;
            case space::ICTCP:
            case space::IPT: throw_not_yet();
            case space::xyY:
            case space::XYZ: return cx::mat();

            case space::LMS_HPE: return _chroma.LMStoXYZ( cone_response::HPE );
            case space::LMS_CAM02:
                return _chroma.LMStoXYZ( cone_response::CIECAM02 );
            case space::LMS_ICTCP:
                // this defines RGB -> LMS
                //Ma = mat( V(1688.0/4096.0), V(2146.0/4096.0), V(262.0/4096.0),
                //V(683.0/4096.0), V(2951.0/4096.0), V(462.0/4096.0),
                //	  V(99.0/4096.0), V(309.0/4096.0), V(3688.0/4096.0) );
                throw_not_yet();

            case space::LMS_OPPONENT:
                return _chroma.LMStoXYZ( cone_response::WANDELL );

            case space::CHONG: throw_not_yet();
            case space::CIE_LAB_76:
            case space::CIE_LUV_76:
            case space::CIE_LCH_76:
            case space::CIE_UVW_64:
            case space::HUNTER_LAB:
                throw_not_yet();
                //return cx::mat();
            case space::UNKNOWN:
                //default:
                break;
        }
        return _chroma.RGBtoXYZ( Y );
    }

    inline cx::mat get_from_xyz_mat( value_type Y = value_type( 1 ) ) const
    {
        switch ( _space )
        {
            case space::RGB:
            case space::YCBCR_BT601:
            case space::YCBCR_BT709:
            case space::YCBCR_BT2020:
            case space::YCBCR_BT2100:
            case space::YCBCR_SYCC:
            case space::YCBCR_CUSTOM:
            case space::HSV_HEX:
            case space::HSV_CYL:
            case space::HSI_HEX:
            case space::HSI_CYL:
            case space::HSL_HEX:
            case space::HSL_CYL: break;
            case space::ICTCP:
            case space::IPT: throw_not_yet();
            case space::xyY:
            case space::XYZ: return cx::mat();

            case space::LMS_HPE: return _chroma.XYZtoLMS( cone_response::HPE );
            case space::LMS_CAM02:
                return _chroma.XYZtoLMS( cone_response::CIECAM02 );
            case space::LMS_ICTCP:
                // this defines RGB -> LMS
                //Ma = mat( V(1688.0/4096.0), V(2146.0/4096.0), V(262.0/4096.0),
                //V(683.0/4096.0), V(2951.0/4096.0), V(462.0/4096.0),
                //	  V(99.0/4096.0), V(309.0/4096.0), V(3688.0/4096.0) );
                throw_not_yet();

            case space::LMS_OPPONENT:
                return _chroma.XYZtoLMS( cone_response::WANDELL );

            case space::CHONG: throw_not_yet();
            case space::CIE_LAB_76:
            case space::CIE_LUV_76:
            case space::CIE_LCH_76:
            case space::CIE_UVW_64:
            case space::HUNTER_LAB:
                throw_not_yet();
                //return cx::mat();
            case space::UNKNOWN:
                //default:
                break;
        }
        return _chroma.XYZtoRGB( Y );
    }

    inline cx::mat adaptation( const state &o, cone_response cr ) const
    {
        return _chroma.adaptation( o.chroma(), value_type( 1 ), cr );
    }

private:
    space                  _space          = space::UNKNOWN;
    range                  _range          = range::FULL;
    transfer               _curve          = transfer::LINEAR;
    cx                     _chroma         = cx();
    value_type             _lum_scale      = value_type( 100.0 );
    value_type             _black_offset   = value_type( 0.0 );
    bool                   _scene_referred = false;
    transfer_curve_control _curve_ctl      = transfer_curve_control();
};

inline std::ostream &operator<<( std::ostream &os, const state &s )
{
    os << "space(" << s.current_space() << ") chroma(" << s.chroma()
       << ") lumscale(" << s.luminance_scale() << ") blackoff("
       << s.black_offset() << ") range(" << s.signal() << ") transfer("
       << s.curve() << ",{" << s.curve_controls() << "})";
    return os;
}

} // namespace color
