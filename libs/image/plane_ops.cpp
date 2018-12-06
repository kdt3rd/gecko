//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include "plane_ops.h"
#include <random>

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

plane create_plane( int x1, int y1, int x2, int y2, float v )
{
	engine::dimensions d;
	d.x1 = static_cast<engine::dimensions::value_type>( x1 );
	d.y1 = static_cast<engine::dimensions::value_type>( y1 );
	d.x2 = static_cast<engine::dimensions::value_type>( x2 );
	d.y2 = static_cast<engine::dimensions::value_type>( y2 );
	d.planes = 1;
	d.bytes_per_item = 4;
	return plane( "p.assign", d, v );
}

////////////////////////////////////////

plane create_plane( int x1, int y1, int x2, int y2, const engine::computed_value<float> &v )
{
	engine::dimensions d;
	d.x1 = static_cast<engine::dimensions::value_type>( x1 );
	d.y1 = static_cast<engine::dimensions::value_type>( y1 );
	d.x2 = static_cast<engine::dimensions::value_type>( x2 );
	d.y2 = static_cast<engine::dimensions::value_type>( y2 );
	d.planes = 1;
	d.bytes_per_item = 4;
	return plane( "p.assign", d, v );
}

////////////////////////////////////////

plane create_random_plane( int x1, int y1, int x2, int y2, uint32_t seed, float minV, float maxV )
{
	engine::dimensions d;
	d.x1 = static_cast<engine::dimensions::value_type>( x1 );
	d.y1 = static_cast<engine::dimensions::value_type>( y1 );
	d.x2 = static_cast<engine::dimensions::value_type>( x2 );
	d.y2 = static_cast<engine::dimensions::value_type>( y2 );
	d.planes = 1;
	d.bytes_per_item = 4;

	return plane( "p.random", d, seed, minV, maxV );
}

////////////////////////////////////////

plane create_iotaX_plane( int x1, int y1, int x2, int y2 )
{
	engine::dimensions d;
	d.x1 = static_cast<engine::dimensions::value_type>( x1 );
	d.y1 = static_cast<engine::dimensions::value_type>( y1 );
	d.x2 = static_cast<engine::dimensions::value_type>( x2 );
	d.y2 = static_cast<engine::dimensions::value_type>( y2 );
	d.planes = 1;
	d.bytes_per_item = 4;

	return plane( "p.iota_x", d );
}

////////////////////////////////////////

plane create_iotaY_plane( int x1, int y1, int x2, int y2 )
{
	engine::dimensions d;
	d.x1 = static_cast<engine::dimensions::value_type>( x1 );
	d.y1 = static_cast<engine::dimensions::value_type>( y1 );
	d.x2 = static_cast<engine::dimensions::value_type>( x2 );
	d.y2 = static_cast<engine::dimensions::value_type>( y2 );
	d.planes = 1;
	d.bytes_per_item = 4;

	return plane( "p.iota_y", d );
}

////////////////////////////////////////

plane pad( const plane &a, int l, int t, int r, int b, float val )
{
	precondition( l >= 0 && t >= 0 && r >= 0 && b >= 0, "expecting to only grow the image in pad, got pad request of {0},{1},{2},{3}", l, t, r, b );
	if ( l == 0 && t == 0 && r == 0 && b == 0 )
		return a;

	engine::dimensions adim = a.dims();
	engine::dimensions d;
	d.x1 = static_cast<engine::dimensions::value_type>( adim.x1 - l );
	d.y1 = static_cast<engine::dimensions::value_type>( adim.y1 - t );
	d.x2 = static_cast<engine::dimensions::value_type>( adim.x2 + r );
	d.y2 = static_cast<engine::dimensions::value_type>( adim.y2 + b );
	d.planes = 1;
	d.bytes_per_item = 4;

	return plane( "p.pad", d, a, val );
}

////////////////////////////////////////

plane pad_hold( const plane &a, int l, int t, int r, int b )
{
	precondition( l >= 0 && t >= 0 && r >= 0 && b >= 0, "expecting to only grow the image in pad, got pad request of {0},{1},{2},{3}", l, t, r, b );
	if ( l == 0 && t == 0 && r == 0 && b == 0 )
		return a;

	engine::dimensions adim = a.dims();
	engine::dimensions d;
	d.x1 = static_cast<engine::dimensions::value_type>( adim.x1 - l );
	d.y1 = static_cast<engine::dimensions::value_type>( adim.y1 - t );
	d.x2 = static_cast<engine::dimensions::value_type>( adim.x2 + r );
	d.y2 = static_cast<engine::dimensions::value_type>( adim.y2 + b );
	d.planes = 1;
	d.bytes_per_item = 4;

	return plane( "p.pad_hold", d, a );
}

////////////////////////////////////////

plane
union_dim( const plane &a, const plane &b, float outside )
{
	engine::dimensions adim = a.dims();
	engine::dimensions bdim = b.dims();

	engine::dimensions r = adim;
	r.x1 = std::min( adim.x1, bdim.x1 );
	r.y1 = std::min( adim.y1, bdim.y1 );
	r.x2 = std::max( adim.x2, bdim.x2 );
	r.y2 = std::max( adim.y2, bdim.y2 );

	if ( r == adim )
		return a;

	return pad( a, adim.x1 - r.x1, adim.y1 - r.y1, r.x2 - adim.x2, r.y2 - adim.y2, outside );
}

////////////////////////////////////////

plane
union_dim_hold( const plane &a, const plane &b )
{
	engine::dimensions adim = a.dims();
	engine::dimensions bdim = b.dims();

	engine::dimensions r = adim;
	r.x1 = std::min( adim.x1, bdim.x1 );
	r.y1 = std::min( adim.y1, bdim.y1 );
	r.x2 = std::max( adim.x2, bdim.x2 );
	r.y2 = std::max( adim.y2, bdim.y2 );

	if ( r == adim )
		return a;

	return pad_hold( a, adim.x1 - r.x1, adim.y1 - r.y1, r.x2 - adim.x2, r.y2 - adim.y2 );
}

////////////////////////////////////////

plane
dirichlet( const plane &p, int border )
{
	return plane( "p.dirichlet", p.dims(), p, border );
}

////////////////////////////////////////

} // namespace image

