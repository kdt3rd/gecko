//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include "image.h"
#include <base/contract.h>

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

image_buf::image_buf( const image_buf &o )
		: computed_base( o ), _planes( o._planes )
{
}

////////////////////////////////////////

image_buf::image_buf( image_buf &&o )
		: computed_base( std::move( o ) ), _planes( std::move( o._planes ) )
{
}

////////////////////////////////////////

image_buf &image_buf::operator=( const image_buf &o )
{
	if ( &o != this )
	{
		internal_copy( o );
		_planes = o._planes;
	}
	return *this;
}

////////////////////////////////////////

image_buf &image_buf::operator=( image_buf &&o )
{
	adopt( std::move( o ) );
	_planes = std::move( o._planes );
	return *this;
}

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
	h << typeid(i).hash_code() << i.size();
	for ( auto &p: i )
		h << p;

	return h;
}

////////////////////////////////////////

} // image



