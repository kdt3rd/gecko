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

#include "image_ops.h"

////////////////////////////////////////

namespace image
{

image_buf
combine( const plane &p )
{
	engine::dimensions id = p.dims();
	id.z = 1;
	id.w = 0;
	return image_buf( "i.combine_p", id, p );
}

image_buf
combine( const plane &p, const plane &p2 )
{
	// is this really a constraint? sometimes...
	// but what about 4:2:0?
	precondition( p.dims() == p2.dims(), "plane dimensions should match to combine to an image" );
	engine::dimensions id = p.dims();
	id.z = 2;
	id.w = 0;
	return image_buf( "i.combine_pp", id, p, p2 );
}

////////////////////////////////////////

image_buf
combine( const plane &p, const plane &p2, const plane &p3 )
{
	// is this really a constraint? sometimes...
	// but what about 4:2:0?
	precondition( p.dims() == p2.dims() && p.dims() == p3.dims(), "plane dimensions should match to combine to an image" );
	engine::dimensions id = p.dims();
	id.z = 3;
	id.w = 0;
	return image_buf( "i.combine_ppp", id, p, p2, p3 );
}

////////////////////////////////////////

image_buf
combine( const plane &p, const plane &p2, const plane &p3, const plane &p4 )
{
	// is this really a constraint? sometimes...
	// but what about 4:2:0?
	precondition( p.dims() == p2.dims() && p.dims() == p3.dims() && p.dims() == p4.dims(), "plane dimensions should match to combine to an image" );
	engine::dimensions id = p.dims();
	id.z = 4;
	id.w = 0;
	return image_buf( "i.combine_pppp", id, p, p2, p3, p4 );
}

////////////////////////////////////////

} // image



