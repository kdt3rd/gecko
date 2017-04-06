//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "plane_ops.h"
#include <random>

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

plane create_random_plane( int w, int h, uint32_t seed, float minV, float maxV )
{
	engine::dimensions d;
	d.x = static_cast<uint16_t>( w );
	d.y = static_cast<uint16_t>( h );

	return plane( "p.random", d, seed, minV, maxV );
}

////////////////////////////////////////

plane create_iotaX_plane( int w, int h )
{
	engine::dimensions d;
	d.x = static_cast<uint16_t>( w );
	d.y = static_cast<uint16_t>( h );

	return plane( "p.iota_x", d );
}

////////////////////////////////////////

plane create_iotaY_plane( int w, int h )
{
	engine::dimensions d;
	d.x = static_cast<uint16_t>( w );
	d.y = static_cast<uint16_t>( h );

	return plane( "p.iota_y", d );
}

////////////////////////////////////////

plane
dirichlet( const plane &p, int border )
{
	return plane( "p.dirichlet", p.dims(), p, border );
}

////////////////////////////////////////

} // namespace image

