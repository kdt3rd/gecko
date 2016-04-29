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

#include <base/const_string.h>
#include "sample_rate.h"
#include "metadata.h"


////////////////////////////////////////


namespace media
{

enum track_type
{
	TRACK_VIDEO,
	TRACK_AUDIO,
	TRACK_DATA
};
	
///
/// @brief Class track_description provides the information necessary
///        to create a new output track. These are combined together to make
///        a new container.
///
class track_description
{
public:
	track_description( track_type tt );
	~track_description( void ) = default;

	inline track_type type( void ) const { return _type; }

	inline void name( base::cstring name ) { _name = name; }
	inline const std::string &name( void ) const { return _name; }

	inline void offset( int64_t o ) { _offset = o; }
	inline int64_t offset( void ) const { return _offset; }
	inline void duration( int64_t num_samples ) { _duration = num_samples; }
	inline int64_t duration( void ) const { return _duration; }
	inline void rate( const sample_rate &sr ) { _sample_rate = sr; }
	const sample_rate &rate( void ) const { return _sample_rate; }

	inline void codec( base::cstring name ) { _codec = name; }
	inline const std::string &codec( void ) const { return _codec; }
			 
	inline void set_option( base::cstring opt, base::any v ) { _options[opt] = std::move( v ); }
	inline const metadata &options( void ) const { return _options; }

	inline void set_meta( base::cstring name, base::any v ) { _metadata[name] = std::move( v ); }
	inline const metadata &meta( void ) const { return _metadata; }

private:
	track_type _type;
	std::string _name;
	int64_t _offset = -1;
	int64_t _duration = -1;
	sample_rate _sample_rate;

	std::string _codec;
	metadata _options;

	metadata _metadata;
};

} // namespace media



