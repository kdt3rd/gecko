// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "sample_rate.h"

#include <base/contract.h>
#include <base/string_util.h>
#include <cmath>
#include <sstream>

////////////////////////////////////////

namespace media
{
////////////////////////////////////////

bool sample_rate::is_drop_frame( void ) const
{
    return (
        _realtime && _ratio.denominator() == 1001 &&
        ( _ratio.numerator() == 30000 || _ratio.numerator() == 60000 ) );
}

////////////////////////////////////////

int64_t sample_rate::resample( int64_t i, const sample_rate &rate ) const
{
    precondition( _ratio.valid(), "invalid destination sample rate" );
    precondition( rate._ratio.valid(), "invalid source sample rate" );
    base::ratio tmp = i * ( _ratio / rate._ratio );
    return tmp.round();
}

////////////////////////////////////////

void sample_rate::set( int64_t n, int64_t d, bool realtime )
{
    _ratio.set( n, d );
    _realtime = realtime;
}

////////////////////////////////////////

void sample_rate::set_rate( double r, bool realtime )
{
    precondition( r > 0.0, "invalid sample rate {0}", r );

    // people will often pass in 23.98 for 23.976
    // so use an epsilon of 0.005
    static const double kRateEps = 0.005;
    if ( std::abs( r - ( 24000.0 / 1001.0 ) ) < kRateEps )
        set( 24000, 1001 );
    else if ( std::abs( r - ( 30000.0 / 1001.0 ) ) < kRateEps )
        set( 30000, 1001 );
    else if ( std::abs( r - ( 48000.0 / 1001.0 ) ) < kRateEps )
        set( 48000, 1001 );
    else if ( std::abs( r - ( 60000.0 / 1001.0 ) ) < kRateEps )
        set( 60000, 1001 );
    else
    {
        // EDL files and such have frame rates
        // for speed changes that are fractional
        int64_t num    = static_cast<int64_t>( r );
        int64_t den    = 1;
        double  curRem = r - static_cast<double>( num );
        while ( std::abs( curRem ) > kRateEps )
        {
            den *= 10;
            num = static_cast<int64_t>( r * static_cast<double>( den ) );
            curRem =
                r - ( static_cast<double>( num ) / static_cast<double>( den ) );
        }

        set( num, den );
    }
    _realtime = realtime;
}

////////////////////////////////////////

void sample_rate::set_rate( const std::string &r )
{
    std::size_t ep = 0;
    double      rn = std::stod( r, &ep );
    bool        rt = false;
    if ( ep < r.size() )
    {
        std::string ss = base::trim( r.substr( ep ) );
        if ( ss == "DF" || ss == "df" )
            rt = true;
    }
    set_rate( rn, rt );
}

////////////////////////////////////////

double sample_rate::to_number( void ) const { return _ratio.value(); }

////////////////////////////////////////

std::string sample_rate::to_string( void ) const
{
    std::stringstream buf;
    if ( !_ratio.valid() )
        buf << "INVALID";
    else if ( _ratio.denominator() == 1 )
        buf << _ratio.numerator();
    else if ( _ratio.denominator() == 1001 )
    {
        switch ( _ratio.numerator() )
        {
            case 24000: buf << "23.976"; break;
            case 30000: buf << "29.97" << ( _realtime ? "DF" : "NDF" ); break;
            case 48000: buf << "47.95"; break;
            case 60000: buf << "59.94" << ( _realtime ? "DF" : "NDF" ); break;
            default: buf << _ratio.value(); break;
        }
    }
    else
        buf << _ratio.value();

    return buf.str();
}

////////////////////////////////////////

sample_rate sample_rate::common( const sample_rate &o ) const
{
    auto x = _ratio.common( o._ratio );
    return sample_rate( x.numerator(), x.denominator() );
}

////////////////////////////////////////

const std::vector<sample_rate> &sample_rate::frame_rates( void )
{
    static std::vector<sample_rate> frates{
        sample_rate( 24000, 1001 ), sample_rate( 24, 1 ),
        sample_rate( 25, 1 ),       sample_rate( 30000, 1001 ),
        sample_rate( 30, 1 ),       sample_rate( 48000, 1001 ),
        sample_rate( 48, 1 ),       sample_rate( 50, 1 ),
        sample_rate( 60000, 1001 ), sample_rate( 60, 1 ),
        sample_rate( 72, 1 ),       sample_rate( 96, 1 ),
        sample_rate( 120, 1 )
    };

    return frates;
}

////////////////////////////////////////

const std::vector<sample_rate> &sample_rate::audio_rates( void )
{
    static std::vector<sample_rate> arates{
        sample_rate( 8000, 1 ),   // telephone
        sample_rate( 16000, 1 ),  // modern VOIP
        sample_rate( 44056, 1 ),  // NTSC color video rate (29.97)
        sample_rate( 44100, 1 ),  // audio CD
        sample_rate( 47250, 1 ),  // first PCM
        sample_rate( 48000, 1 ),  // most professional audio PCM
        sample_rate( 88200, 1 ),  // double audio
        sample_rate( 96000, 1 ),  // DVD / blu-ray / etc. audio (2x 48kHz)
        sample_rate( 192000, 1 ), // DVD / blu-ray / etc. audio (4x 48kHz)
    };

    return arates;
}

////////////////////////////////////////

} // namespace media
