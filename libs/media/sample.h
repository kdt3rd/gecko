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

private:
	int64_t _offset = -1;
	sample_rate _rate;
};

} // namespace media



