//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once

#include "sample.h"
#include <base/const_string.h>


////////////////////////////////////////


namespace media
{

///
/// @brief time_code provides a simple wrapper around a time
///
/// should handle drop frame
class time_code
{
public:
	inline bool valid( void ) const;

	inline void set( int hr, int mins, int sec, int frm = 0 );
	inline void set( int hr, int mins, int sec, int frm, const sample_rate &sr );

	inline void hours( int hr );
	inline int hours( void ) const;
	inline void minutes( int mins );
	inline int minutes( void ) const;
	inline void seconds( int sec );
	inline int seconds( void ) const;
	inline void frames( int fr );
	inline int frames( void ) const;

	inline const sample_rate &parse_rate( void ) const;

	int64_t to_frame( const sample_rate &sr ) const;
	void from_frame( int64_t f, const sample_rate &sr );
	inline sample to_sample( const sample_rate &sr ) const;
	inline void from_sample( const sample &s );

	/// DPX files have it this way
	void from_smpte( uint32_t tc );
	uint32_t to_smpte( void ) const;
	void from_smpte_rp188( uint32_t low, uint32_t high, const sample_rate &sr );
	std::pair<uint32_t, uint32_t> to_smpte_rp188( const sample_rate &sr ) const;

	std::string to_string( const sample_rate &sr,
						   bool edlNotation = false ) const;
	void from_string( const base::cstring &cs,
					  const sample_rate &sr,
					  bool edlNotation = false );
private:
	int _hours = -1;
	int _minutes = -1;
	int _seconds = -1;
	int _frames = -1;
	sample_rate _rate;
};

std::istream &operator>>( std::istream &in, time_code &tc );
std::ostream &operator<<( std::ostream &out, const time_code &tc );

inline bool operator<( const time_code &a, const time_code &b )
{
	// don't care about the parse rate
	return ( a.hours() < b.hours() ||
			 ( a.hours() == b.hours() &&
			   ( a.minutes() < b.minutes() ||
				 ( a.minutes() == b.minutes() &&
				   ( a.seconds() < b.seconds() ||
					 ( a.seconds() == b.seconds() &&
					   a.frames() < b.frames() ) ) ) ) ) );
}

inline bool operator==( const time_code &a, const time_code &b )
{
	// don't care about the parse rate
	return ( a.hours() == b.hours() && a.minutes() == b.minutes() &&
			 a.seconds() == b.seconds() && a.frames() == b.frames() );
}

inline bool operator!=( const time_code &a, const time_code &b )
{
	return !( a == b );
}


////////////////////////////////////////


inline bool
time_code::valid( void ) const
{
	return hours() >= 0 && minutes() >= 0 && seconds() >= 0 && frames() >= 0;
}

inline void
time_code::set( int hr, int mins, int sec, int frm )
{
	_hours = hr;
	_minutes = mins;
	_seconds = sec;
	_frames = frm;
}

inline void
time_code::set( int hr, int mins, int sec, int frm, const sample_rate &sr )
{
	_hours = hr;
	_minutes = mins;
	_seconds = sec;
	_frames = frm;
	_rate = sr;
}


////////////////////////////////////////


inline void time_code::hours( int hr )
{ _hours = hr; }
inline int time_code::hours( void ) const
{ return _hours; }
inline void time_code::minutes( int mins )
{ _minutes = mins; }
inline int time_code::minutes( void ) const
{ return _minutes; }
inline void time_code::seconds( int sec )
{ _seconds = sec; }
inline int time_code::seconds( void ) const
{ return _seconds; }
inline void time_code::frames( int fr )
{ _frames = fr; }
inline int time_code::frames( void ) const
{ return _frames; }
inline const sample_rate &time_code::parse_rate( void ) const
{ return _rate; }



////////////////////////////////////////


inline sample
time_code::to_sample( const sample_rate &sr ) const
{
	return sample( to_frame( sr ), sr );
}
inline void
time_code::from_sample( const sample &s )
{
	return from_frame( s.offset(), s.rate() );
}



} // namespace media



