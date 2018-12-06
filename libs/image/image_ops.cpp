//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include "image_ops.h"
#include "plane_ops.h"

////////////////////////////////////////

namespace image
{

image_buf
combine( const plane &p )
{
	engine::dimensions id = p.dims();
	id.planes = 1;
	id.images = 1;
	return image_buf( "i.combine_p", id, p );
}

image_buf
combine( const plane &p, const plane &p2 )
{
	// is this really a constraint? sometimes...
	// but what about 4:2:0?
	precondition( p.dims() == p2.dims(), "plane dimensions should match to combine to an image" );
	engine::dimensions id = p.dims();
	id.planes = 2;
	id.images = 1;
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
	id.planes = 3;
	id.images = 1;
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
	id.planes = 4;
	id.images = 1;
	return image_buf( "i.combine_pppp", id, p, p2, p3, p4 );
}

////////////////////////////////////////

image_buf
union_dim( const image_buf &a, const image_buf &b, float outside )
{
	image_buf ret = a;
	for ( int p = 0; p < ret.planes(); ++p )
		ret[p] = union_dim( a[p], b[p], outside );
	return ret;
}

////////////////////////////////////////

image_buf
union_dim_hold( const image_buf &a, const image_buf &b )
{
	image_buf ret = a;
	for ( int p = 0; p < ret.planes(); ++p )
		ret[p] = union_dim_hold( a[p], b[p] );
	return ret;
}

////////////////////////////////////////

} // image



