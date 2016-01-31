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

#include "sample_rate.h"
#include <base/contract.h>
#include <cmath>
#include <sstream>


////////////////////////////////////////


namespace media
{


////////////////////////////////////////


sample_rate::sample_rate( int64_t n, int64_t d )
		: _ratio( n, d )
{
}


////////////////////////////////////////


bool
sample_rate::is_drop_frame( void ) const
{
	return ( _ratio.denominator() == 1001 &&
			 ( _ratio.numerator() == 30000 || _ratio.numerator() == 60000 ) );
}


////////////////////////////////////////


int64_t
sample_rate::resample( int64_t i, const sample_rate &rate ) const
{
	precondition( _ratio.valid(), "invalid destination sample rate" );
	precondition( rate._ratio.valid(), "invalid source sample rate" );
	base::ratio tmp = i * ( _ratio / rate._ratio );
	return tmp.round();
}


////////////////////////////////////////


void
sample_rate::set( int64_t n, int64_t d )
{
	_ratio.set( n, d );
}


////////////////////////////////////////


void
sample_rate::set_rate( double r )
{
	precondition( r > 0.0, "invalid sample rate {0}", r );
	// we could loop over all the frame rates
	// and audio rates, but with the exception
	// of the frame rates that are 1001 denoms,
	// all are whole numbers, so just check for
	// the 1001 cases for now

	// people will often pass in 23.98 for 23.976
	// so use an epsilon of 0.01
	static const double kRateEps = 0.01;
	if ( std::abs( r - (24000.0/1001.0) ) < kRateEps )
		set( 24000, 1001 );
	else if ( std::abs( r - (30000.0/1001.0) ) < kRateEps )
		set( 30000, 1001 );
	else if ( std::abs( r - (48000.0/1001.0) ) < kRateEps )
		set( 48000, 1001 );
	else if ( std::abs( r - (60000.0/1001.0) ) < kRateEps )
		set( 60000, 1001 );
	else
		set( static_cast<int64_t>( r + 0.5 ), 1 );
}


////////////////////////////////////////


void
sample_rate::set_rate( const std::string &r )
{
	set_rate( std::stod( r ) );
}


////////////////////////////////////////


double
sample_rate::as_number( void ) const
{
	return _ratio.value();
}


////////////////////////////////////////


std::string
sample_rate::as_string( void ) const
{
	std::stringstream buf;
	if ( ! _ratio.valid() )
		buf << "INVALID";
	else if ( _ratio.denominator() == 1 )
		buf << _ratio.numerator();
	else if ( _ratio.denominator() == 1001 )
	{
		switch ( _ratio.numerator() )
		{
			case 24000: buf << "23.976"; break;
			case 30000: buf << "29.97"; break;
			case 48000: buf << "47.95"; break;
			case 60000: buf << "59.94"; break;
			default:
				buf << _ratio.value();
				break;
		}
	}
	else
		buf << _ratio.value();

	return buf.str();
}


////////////////////////////////////////


sample_rate
sample_rate::common( const sample_rate &o ) const
{
	auto x = _ratio.common( o._ratio );
	return sample_rate( x.first.numerator(), x.first.denominator() );
}


////////////////////////////////////////


const std::vector<sample_rate> &
sample_rate::frame_rates( void )
{
	static std::vector<sample_rate> frates 
	{
		sample_rate( 24000, 1001 ),
		sample_rate( 24, 1 ),
		sample_rate( 25, 1 ),
		sample_rate( 30000, 1001 ),
		sample_rate( 30, 1 ),
		sample_rate( 48000, 1001 ),
		sample_rate( 48, 1 ),
		sample_rate( 50, 1 ),
		sample_rate( 60000, 1001 ),
		sample_rate( 60, 1 ),
		sample_rate( 72, 1 ),
		sample_rate( 96, 1 ),
		sample_rate( 120, 1 )
	};

	return frates;
}


////////////////////////////////////////


const std::vector<sample_rate> &
sample_rate::audio_rates( void )
{
	static std::vector<sample_rate> arates 
	{
		sample_rate( 8000, 1 ), // telephone
		sample_rate( 16000, 1 ), // modern VOIP
		sample_rate( 44056, 1 ), // NTSC color video rate (29.97)
		sample_rate( 44100, 1 ), // audio CD
		sample_rate( 47250, 1 ), // first PCM
		sample_rate( 48000, 1 ), // most professional audio PCM
		sample_rate( 88200, 1 ), // double audio
		sample_rate( 96000, 1 ), // DVD / blu-ray / etc. audio (2x 48kHz)
		sample_rate( 192000, 1 ), // DVD / blu-ray / etc. audio (4x 48kHz)
	};

	return arates;
}


////////////////////////////////////////


} // media



