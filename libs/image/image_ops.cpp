//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
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



