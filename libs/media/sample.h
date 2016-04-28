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

#include "sample_rate.h"
#include "sample_data.h"
#include <utility>
#include <memory>


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

} // namespace media



