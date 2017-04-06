//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "sample_rate.h"
#include "sample_data.h"
#include <utility>
#include <memory>
#include <ostream>


////////////////////////////////////////

namespace media
{

///
/// @brief Class sample provides...
///
class sample
{
public:
	inline sample( void ) = default;
	inline sample( int64_t o, const sample_rate &sr ) : _offset( o ), _rate( sr ) {}
	inline ~sample( void ) = default;
	inline sample( const sample & ) = default;
	inline sample &operator=( const sample & ) = default;
	inline sample( sample && ) = default;
	inline sample &operator=( sample && ) = default;

	inline int64_t offset( void ) const { return _offset; }
	inline const sample_rate &rate( void ) const { return _rate; }

	template <typename T>
	typename std::shared_ptr<typename std::remove_pointer<decltype( std::declval<T>().read(int64_t(), sample_rate()) )>::type> operator()( const std::shared_ptr<T> &track ) const
	{
		typedef decltype( std::declval<T>().read(int64_t(), sample_rate()) ) ret_ptr;
		typedef typename std::remove_pointer<ret_ptr>::type ret_type;
		static_assert( std::is_base_of<sample_data, ret_type>::value, "Track object read value no derived from sample_data" );
		return std::shared_ptr<ret_type>( track->read( _offset, _rate ) );
	}

private:
	int64_t _offset = -1;
	sample_rate _rate;
};

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &os, const sample &s )
{
	os << s.offset() << '@' << s.rate();
	return os;
}

} // namespace media



