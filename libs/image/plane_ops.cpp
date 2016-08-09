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

