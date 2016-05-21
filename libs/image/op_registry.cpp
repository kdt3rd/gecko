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

#include "op_registry.h"
#include "image.h"
//#include "image_ops.h"
#include "plane_ops.h"

#include <mutex>

////////////////////////////////////////

namespace
{
using namespace image;

////////////////////////////////////////

image_buf combine_planes_1( const plane &p )
{
	image_buf r;
	r.add_plane( p );
	return r;
}

////////////////////////////////////////

image_buf combine_planes_2( const plane &p1, const plane &p2 )
{
	image_buf r;
	r.add_plane( p1 );
	r.add_plane( p2 );
	return r;
}

////////////////////////////////////////

image_buf combine_planes_3( const plane &p1, const plane &p2, const plane &p3 )
{
	image_buf r;
	r.add_plane( p1 );
	r.add_plane( p2 );
	r.add_plane( p3 );
	return r;
}

////////////////////////////////////////

image_buf combine_planes_4( const plane &p1, const plane &p2, const plane &p3, const plane &p4 )
{
	image_buf r;
	r.add_plane( p1 );
	r.add_plane( p2 );
	r.add_plane( p3 );
	r.add_plane( p4 );
	return r;
}

////////////////////////////////////////

plane extract_plane( const image_buf &i, size_t idx )
{
	return i[idx];
}

////////////////////////////////////////

void
registerImageOps( engine::registry &r )
{
	using namespace engine;
	r.register_constant<image::image_buf>();
	r.add( op( "img_extract_plane", extract_plane, op::single_threaded ) );
}

////////////////////////////////////////

void
registerPlaneOps( engine::registry &r )
{
	r.register_constant<image::plane>();
}

////////////////////////////////////////

std::once_flag initOpsFlag;
std::unique_ptr<engine::registry> imageRegistry;

void shutdown( void )
{
	imageRegistry.reset();
}

void initOps( void )
{
	imageRegistry = std::make_unique<engine::registry>();
	std::atexit( &shutdown );

	registerPlaneOps( *imageRegistry );
	registerImageOps( *imageRegistry );
}

}


////////////////////////////////////////

namespace image
{

////////////////////////////////////////

engine::registry &
op_registry( void )
{
	std::call_once( initOpsFlag, initOps );
	return *imageRegistry;
}

////////////////////////////////////////

} // image



