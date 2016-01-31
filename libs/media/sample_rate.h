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
	sample_rate( int64_t n, int64_t d = 1 );
	sample_rate( const sample_rate & ) = default;
	sample_rate( sample_rate && ) = default;
	~sample_rate( void ) = default;
	sample_rate &operator=( const sample_rate & ) = default;
	sample_rate &operator=( sample_rate && ) = default;

	bool is_drop_frame( void ) const;

	/// @brief Resamples the incoming sample with it's rate at
	/// the rate represented by this
	///
	/// @param i sample to be resampled
	/// @param rate rate of incoming sample
	///
	/// @return sample number at the current rate
	int64_t resample( int64_t i, const sample_rate &rate ) const;

	/// @brief re-assigns the sample rate
	void set( int64_t n, int64_t d );

	/// @brief Sets a sample rate based on inexact non-rational
	/// numbers.
	///
	/// This uses the appropriate common rates to find an exact
	/// match. for example, setting a sample_rate to 23.98 as a double
	/// would result in 24000/1001 since that is the nearest common
	/// rate for a frame rate
	///
	/// if no common rate is found, will assign to the closest appropriate whole number.
	void set_rate( double r );

	/// @brief utility to convert string to double and recurse to other set_rate
	void set_rate( const std::string &r );

	double as_number( void ) const;
	std::string as_string( void ) const;

	sample_rate common( const sample_rate &o ) const;

	/// @brief common frame rates
	static const std::vector<sample_rate> &frame_rates( void );
	/// @brief common audio rates
	static const std::vector<sample_rate> &audio_rates( void );

private:
	base::ratio _ratio;
};

inline std::ostream &operator<<( std::ostream &os, const sample_rate &r )
{
	os << r.as_string();
	return os;
}


} // namespace media



