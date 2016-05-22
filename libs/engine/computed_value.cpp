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

#include "computed_value.h"

////////////////////////////////////////

namespace engine
{

////////////////////////////////////////

computed_base::~computed_base( void )
{
	clear_graph();
}

////////////////////////////////////////

dimensions
computed_base::node_dims( void ) const
{
	dimensions r = nulldim;
	if ( _graph )
		r = (*_graph)[_id].dims();
	return r;
}

////////////////////////////////////////

bool
computed_base::pending( void ) const
{
	if ( _graph )
	{
		const node &n = (*_graph)[_id];
		if ( n.value().empty() )
			return true;
	}

	return false;
}

////////////////////////////////////////

const any &
computed_base::compute( void ) const
{
	if ( ! _graph )
		throw_runtime( "No graph to compute with" );
	return _graph->get_value( _id );
}

////////////////////////////////////////

void
computed_base::clear_graph( void ) noexcept
{
	set_id( nullnode );
	_graph.reset();
}

////////////////////////////////////////

bool computed_base::compute_hash( hash &v ) const
{
	if ( _graph )
	{
		v << (*_graph)[_id].hash_value();
		return true;
	}
	return false;
}

////////////////////////////////////////

void
computed_base::set_id( node_id i )
{
	if ( _id != nullnode )
	{
		if ( _graph )
			_graph->unreference( _id );
	}
	_id = i;
	if ( _id != nullnode )
	{
		if ( _graph )
			_graph->reference( _id );
	}
}

////////////////////////////////////////

void
computed_base::adopt( computed_base &&o )
{
	clear_graph();
	// don't have to change reference count as we're adopting
	std::swap( _graph, o._graph );
	std::swap( _id, o._id );
}

////////////////////////////////////////

} // engine



