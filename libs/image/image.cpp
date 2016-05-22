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

#include "image.h"
#include <base/contract.h>

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

image_buf::~image_buf( void )
{
}

////////////////////////////////////////

void
image_buf::add_plane( const plane &p )
{
	if ( ! _planes.empty() )
		precondition( _planes.front().width() == p.width() &&
					  _planes.front().height() == p.height(), "Attempt to create image with mis-matched plane dimensions" );
	_planes.push_back( p );
}

////////////////////////////////////////

void
image_buf::add_plane( plane &&p )
{
	if ( ! _planes.empty() )
		precondition( _planes.front().width() == p.width() &&
					  _planes.front().height() == p.height(), "Attempt to create image with mis-matched plane dimensions" );
	_planes.emplace_back( std::move( p ) );
}

////////////////////////////////////////

engine::hash &
operator<<( engine::hash &h, const image_buf &i )
{
	h << i.size();
	for ( auto &p: i )
		h << p;

	return h;
}

////////////////////////////////////////

} // image



