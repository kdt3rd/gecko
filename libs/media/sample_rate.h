//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <base/ratio.h>
#include <vector>
#include <ostream>


////////////////////////////////////////


namespace media
{

///
/// @brief Provides a rational (not floating point) representation
/// of sample rate.
///
/// This is the basic unit at which frames or other similar data
/// (audio, etc.) should be presented to the user. This is stored as a
/// numerator / denominator to correctly avoid rounding issues when
/// mixing media at different rates and translating ranges
/// between rates.
///
/// The number is in the form of samples per time, so 24 / 1 or
/// 24000 / 1001 meaning 24fps and 23.976... respectively.
/// 
///
class sample_rate
{
public:
	sample_rate( void ) = default;
	/// if realtime is true, enables drop frame handling
	inline constexpr sample_rate( int64_t n, int64_t d = 1, bool realtime = false ) : _ratio( n, d ), _realtime( realtime ) {}
	sample_rate( const sample_rate & ) = default;
	sample_rate( sample_rate && ) = default;
	~sample_rate( void ) = default;
	sample_rate &operator=( const sample_rate & ) = default;
	sample_rate &operator=( sample_rate && ) = default;

	inline constexpr bool valid( void ) const;

	bool is_drop_frame( void ) const;

	inline constexpr bool realtime( void ) const;
	inline constexpr const base::ratio &ratio( void ) const;
	inline constexpr int64_t numerator( void ) const;
	inline constexpr int64_t denominator( void ) const;

	/// @brief Resamples the incoming sample with it's rate at
	/// the rate represented by this
	///
	/// @param i sample to be resampled
	/// @param rate rate of incoming sample
	///
	/// @return sample number at the current rate
	int64_t resample( int64_t i, const sample_rate &rate ) const;

	/// @brief re-assigns the sample rate
	/// if realtime is true, enables drop frame handling
	void set( int64_t n, int64_t d, bool realtime = false );

	/// @brief Sets a sample rate based on inexact non-rational
	/// numbers.
	///
	/// This uses the appropriate common rates to find an exact
	/// match. for example, setting a sample_rate to 23.98 as a double
	/// would result in 24000/1001 since that is the nearest common
	/// rate for a frame rate
	/// if realtime is true, enables drop frame handling
	///
	/// if no common fractional frame rate is found, will assign to
	/// the closest appropriate ratio to the precision of 0.005.
	void set_rate( double r, bool realtime = false );

	/// @brief utility to convert string to double and pass to other set_rate
	/// if string has "DF" at the end, enables drop frame handling
	void set_rate( const std::string &r );

	double to_number( void ) const;
	std::string to_string( void ) const;

	sample_rate common( const sample_rate &o ) const;

	/// @brief common frame rates
	static const std::vector<sample_rate> &frame_rates( void );
	/// @brief common audio rates
	static const std::vector<sample_rate> &audio_rates( void );

private:
	base::ratio _ratio;
	bool _realtime = false;
};

////////////////////////////////////////

inline constexpr bool operator==( const sample_rate &a, const sample_rate &b )
{
	return a.ratio() == b.ratio() && a.realtime() == b.realtime();
}

inline constexpr bool operator!=( const sample_rate &a, const sample_rate &b )
{
	return !( a == b );
}

inline constexpr bool operator<( const sample_rate &a, const sample_rate &b )
{
	return ( a.ratio() < b.ratio() ? true :
			 ( a.ratio() == b.ratio() ? a.realtime() < b.realtime() : false ) );
}

inline bool operator>( const sample_rate &a, const sample_rate &b )
{
	return b < a;
}

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &os, const sample_rate &r )
{
	os << r.to_string();
	return os;
}

////////////////////////////////////////


inline constexpr bool
sample_rate::valid( void ) const
{
	return _ratio.valid();
}

inline constexpr bool
sample_rate::realtime( void ) const
{
	return _realtime;
}

inline constexpr const base::ratio &sample_rate::ratio( void ) const
{
	return _ratio;
}

inline constexpr int64_t
sample_rate::numerator( void ) const
{
	return _ratio.numerator();
}

inline constexpr int64_t
sample_rate::denominator( void ) const
{
	return _ratio.denominator();
}

} // namespace media



