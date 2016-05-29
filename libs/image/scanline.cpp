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

#include "scanline.h"
#include <base/pointer.h>

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

scanline::scanline( const float *b, int w, int s, bool dup )
	: _ref_ptr( b ), _width( w ), _stride( s )
{
	if ( dup )
	{
		_ptr = allocator::get().scanline( _stride, _width );
		if ( _ref_ptr )
			std::copy( _ref_ptr, _ref_ptr + _width, _ptr.get() );
		_ref_ptr = _ptr.get();
	}
}

////////////////////////////////////////

scanline::scanline( float *b, int w, int s )
	: _ptr( b, base::no_deleter() ), _ref_ptr( b ), _width( w ), _stride( s )
{
}

////////////////////////////////////////

scanline::scanline( int w )
	: _ptr( allocator::get().scanline( _stride, w ) ), _ref_ptr( _ptr.get() ), _width( w )
{
}

////////////////////////////////////////

void
scanline::swap( scanline &o )
{
	std::swap( _ptr, o._ptr );
	std::swap( _ref_ptr, o._ref_ptr );
	std::swap( _width, o._width );
	std::swap( _stride, o._stride );
}

////////////////////////////////////////

} // image



