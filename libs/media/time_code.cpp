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

#include "time_code.h"
#include <base/contract.h>
#include <stdexcept>
#include <base/string_util.h>


////////////////////////////////////////


namespace media
{


////////////////////////////////////////


int64_t
time_code::to_frame( const sample_rate &sr ) const
{
	if ( ! valid() )
		throw std::logic_error( "Attempt to access uninitialized time_code object" );

	int64_t secs = _hours * 3600 + _minutes * 60 + _seconds;
	if ( sr.is_drop_frame() )
	{
		if ( sr.ratio().numerator() == 30000 )
			return _frames + secs * 30 - ( _hours * 60 + _minutes - _hours * 6 - _minutes / 10 ) * 2;
		else
			return _frames + secs * 60 - ( _hours * 60 + _minutes - _hours * 6 - _minutes / 10 ) * 4;
	}

	// sample_rate is in n / second
	// so use seconds and add frames, then round
	return base::ratio( secs * sr.ratio().numerator() + _frames * sr.ratio().denominator(), sr.ratio().denominator(), false ).round();
}


////////////////////////////////////////


void
time_code::from_frame( int64_t f, const sample_rate &sr )
{
	if ( sr.is_drop_frame() )
	{
		// for 30:
		// every 10th minute has all 1800 frames, 9 inbetween have 1798
		// static int framespermin[10] = { 1800, 1798, 1798, 1798, 1798, 1798, 1798, 1798, 1798, 1798 };
		int fPer10 = 17982;
		int fPerSec = 30;
		int baseFpermin = 1798;
		int skipF = 2;
		if ( sr.numerator() == 60000 )
		{
			fPer10 = 35964;
			fPerSec = 60;
			baseFpermin = 3596;
			skipF = 4;
		}
		int64_t tenminchunks = f / fPer10;
		f = f - tenminchunks * fPer10;

		_hours = static_cast<int>( tenminchunks / 6 );
		_minutes = static_cast<int>( ( tenminchunks - _hours * 6 ) * 10 );

		int mins = static_cast<int>( f - skipF ) / baseFpermin;
		_minutes += mins;
		if ( _minutes >= 60 )
		{
			++_hours;
			_minutes -= 60;
		}
		f -= skipF * (mins > 0 ? 1 : 0);
		f -= mins * baseFpermin;

		_seconds = static_cast<int>( f / fPerSec );
		_frames = static_cast<int>( f - _seconds * fPerSec );
		return;
	}

	base::ratio seconds = f / sr.ratio();
	_seconds = static_cast<int>( seconds.trunc() );
	_frames = static_cast<int>( seconds.mod() / sr.ratio().denominator() );
	_hours = _seconds / 3600;
	_seconds -= _hours * 3600;
	_minutes = _seconds / 60;
	_seconds -= _minutes * 60;
}


////////////////////////////////////////


void
time_code::from_smpte( uint32_t tc )
{
	_hours = ( ( tc >> 28 ) & 0xF ) * 10 + ( ( tc >> 24 ) & 0xF );
	_minutes = ( ( tc >> 20 ) & 0xF ) * 10 + ( ( tc >> 16 ) & 0xF );
	_seconds = ( ( tc >> 12 ) & 0xF ) * 10 + ( ( tc >> 8 ) & 0xF );
	_frames = ( ( tc >> 4 ) & 0xF ) * 10 + ( tc & 0xF );
}


////////////////////////////////////////


uint32_t
time_code::to_smpte( void ) const
{
	uint32_t hrs = static_cast<uint32_t>( ( ( _hours / 10 ) << 4 ) | ( _hours % 10 ) );
	uint32_t min = static_cast<uint32_t>( ( ( _minutes / 10 ) << 4 ) | ( _minutes % 10 ) );
	uint32_t sec = static_cast<uint32_t>( ( ( _seconds / 10 ) << 4 ) | ( _seconds % 10 ) );
	uint32_t frm = static_cast<uint32_t>( ( ( _frames / 10 ) << 4 ) | ( _frames % 10 ) );
	return ( hrs << 24 ) | ( min << 16 ) | ( sec << 8 ) | frm;
}


////////////////////////////////////////


void
time_code::from_smpte_rp188( uint32_t low, uint32_t high, const sample_rate &sr )
{
	_hours = ( ( ( high >> 24 ) & 0x7 ) * 10 ) + ( ( high >> 16 ) & 0xF );
	_minutes = ( ( ( high >> 8 ) & 0x7 ) * 10 ) + ( high & 0xF );
	_seconds = ( ( ( low >> 24 ) & 0x7 ) * 10 ) + ( ( low >> 16 ) & 0xF );
	_frames = ( ( ( low >> 8 ) & 0x3 ) * 10 ) + ( low & 0xF );
	// hrm, don't know pal vs ntsc, assume ntsc?
	if ( sr.to_number() > 39.0 )
	{
		// extra bit for frame 10s
		_frames = _frames * 2 + static_cast<int>( (low >> 27) & 0x01 );
	}
}


////////////////////////////////////////


std::pair<uint32_t, uint32_t>
time_code::to_smpte_rp188( const sample_rate &sr ) const
{
	uint32_t low, high;
	if ( sr.to_number() > 39.0 )
		low = static_cast<uint32_t>( ( ( _frames % 2 ) << 27 ) | ( ( _frames / 20 ) << 8 ) | ( ( _frames / 2 ) % 10 ) );
	else
		low = static_cast<uint32_t>( ( ( _frames / 10 ) << 8 ) | ( _frames % 10 ) );
	low |= static_cast<uint32_t>( ( ( _seconds / 10 ) << 24 ) | ( ( _seconds % 10 ) << 16 ) );
	high = static_cast<uint32_t>( ( ( _minutes / 10 ) << 8 ) | ( _minutes % 10 ) );
	high |= static_cast<uint32_t>( ( ( _hours / 10 ) << 24 ) | ( ( _hours % 10 ) << 16 ) );

	return std::make_pair( low, high );
}


////////////////////////////////////////


std::string
time_code::to_string( const sample_rate &sr, bool edlNotation ) const
{
	// HH:MM:SS:FF[*]
	std::string ret;
	std::string retval( 11, ':' );
	retval[0] = char( int( _hours / 10 ) + '0' );
	retval[1] = char( int( _hours % 10 ) + '0' );
	retval[3] = char( int( _minutes / 10 ) + '0' );
	retval[4] = char( int( _minutes % 10 ) + '0' );
	retval[6] = char( int( _seconds / 10 ) + '0' );
	retval[7] = char( int( _seconds % 10 ) + '0' );
	if ( sr.is_drop_frame() )
		retval[8] = ';';

	if ( edlNotation )
	{
		// EDL only allow 0 - 29 as frame marker
		double frate = sr.to_number();
		if ( frate > 60.0 )
			throw std::runtime_error( "EDL notation not supported from frame rates in excess of 60FPS" );

		int f = _frames;
		if ( frate > 30.0 )
		{
			f /= 2;
			if ( 1 == ( _frames - (f*2) ) )
				retval.push_back( '*' );
		}

		retval[9] = char( int( f / 10 ) + '0' );
		retval[10] = char( int( f % 10 ) + '0' );
	}
	else
	{
		retval[9] = char( int( _frames / 10 ) + '0' );
		retval[10] = char( int( _frames % 10 ) + '0' );
	}
	return retval;
}


////////////////////////////////////////


void
time_code::from_string( const base::cstring &cs,
						const sample_rate &sr, bool edlNotation )
{
	_hours = 0;
	_minutes = 0;
	_seconds = 0;
	_frames = 0;
	size_t pos = 0;
	while ( pos < cs.size() && std::isdigit( cs[pos] ) )
	{
		_hours = ( _hours * 10 ) + base::from_digit( cs[pos] );
		++pos;
	}
	if ( pos == cs.size() )
		return;
	if ( cs[pos] == ':' || cs[pos] == ';' || cs[pos] == '.' )
	{
		++pos;
		if ( ( pos + 2 ) >= cs.size() )
			throw std::runtime_error( "Found hours:minutes separator, but expecting 2 digits for minutes" );
		_minutes = base::from_digit( cs[pos] ) * 10 + base::from_digit( cs[pos + 1] );
		pos += 2;
	}
	else
		throw std::runtime_error( "Expecting time_code separator (':', ';', '.')" );
	if ( pos == cs.size() )
		return;
	if ( cs[pos] == ':' || cs[pos] == ';' || cs[pos] == '.' )
	{
		++pos;
		if ( ( pos + 2 ) >= cs.size() )
			throw std::runtime_error( "Found minutes:seconds separator, but expecting 2 digits for seconds" );
		_seconds = base::from_digit( cs[pos] ) * 10 + base::from_digit( cs[pos + 1] );
		pos += 2;
	}
	else
		throw std::runtime_error( "Expecting time_code separator (':', ';', '.')" );

	if ( pos == cs.size() )
		return;
	if ( cs[pos] == ':' || cs[pos] == ';' || cs[pos] == '.' )
	{
		++pos;
		if ( ( pos + 2 ) >= cs.size() )
			throw std::runtime_error( "Found seconds:frames separator, but expecting 2 digits for frames" );
		_seconds = base::from_digit( cs[pos] ) * 10 + base::from_digit( cs[pos + 1] );
		pos += 2;
	}
	else
		throw std::runtime_error( "Expecting time_code separator (':', ';', '.')" );

	if ( pos == cs.size() )
		return;

	if ( edlNotation )
	{
		double frate = sr.to_number();
		if ( frate > 60.0 )
			throw std::runtime_error( "EDL notation not supported from frame rates in excess of 60FPS" );
		if ( frate > 30.0 )
		{
			_frames *= 2;
			if ( cs[pos] == '*' )
				++_frames;
			else if ( cs[pos] == '.' )
			{
				if ( pos + 1 < cs.size() )
				{
					if ( cs[pos+1] == '1' )
						++_frames;
					else if ( cs[pos+1] != '0' )
						throw std::runtime_error( "Invalid field marker" );
				}
				else
					throw std::runtime_error( "missing data after field marker" );
			}
		}
	}
}


////////////////////////////////////////


std::istream &operator>>( std::istream &in, time_code &tc )
{
    std::istream::sentry s( in, true );
    if ( ! s )
	{
		tc.set( -1, -1, -1, -1 );
		return in;
	}

	char x = 0;
	int hrs = 0;
	int mins = 0;
	int secs = 0;
	int frms = 0;
	while ( in )
	{
		x = 0;
		in >> x;
		if ( ! std::isdigit( x ) )
			break;
		hrs = ( hrs * 10 ) + base::from_digit( x );
	}
	if ( ! in )
		return in;

	if ( x == ':' || x == ';' || x == '.' )
	{
		char m1 = 0, m2 = 0;
		in >> m1 >> m2;
		if ( in && std::isdigit( m1 ) && std::isdigit( m2 ) )
			mins = base::from_digit( m1 ) * 10 + base::from_digit( m2 );
		else
			throw std::runtime_error( "Unable to extract 2 digits for minutes field of time_code" );
		in >> x;
	}
	else
		throw std::runtime_error( "Expecting time_code separator (':', ';', '.')" );

	if ( ! in )
		return in;
	if ( x == ':' || x == ';' || x == '.' )
	{
		char s1 = 0, s2 = 0;
		in >> s1 >> s2;
		if ( in && std::isdigit( s1 ) && std::isdigit( s2 ) )
			secs = base::from_digit( s1 ) * 10 + base::from_digit( s2 );
		else
			throw std::runtime_error( "Unable to extract 2 digits for seconds field of time_code" );
		in >> x;
	}
	else
		throw std::runtime_error( "Expecting time_code separator (':', ';', '.')" );

	if ( ! in )
		return in;
	if ( x == ':' || x == ';' || x == '.' )
	{
		char f1 = 0, f2 = 0;
		in >> f1 >> f2;
		if ( in && std::isdigit( f1 ) && std::isdigit( f2 ) )
			frms = base::from_digit( f1 ) * 10 + base::from_digit( f2 );
		else
			throw std::runtime_error( "Unable to extract 2 digits for frames field of time_code" );
	}
	else
		throw std::runtime_error( "Expecting time_code separator (':', ';', '.')" );

	tc.set( hrs, mins, secs, frms );
	return in;
}


////////////////////////////////////////


std::ostream &operator<<( std::ostream &out, const time_code &tc )
{
	out << tc.to_string( tc.parse_rate() );
	return out;
}


////////////////////////////////////////


} // media



