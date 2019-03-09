// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "computed_value.h"
#include <mutex>

////////////////////////////////////////

namespace engine
{

////////////////////////////////////////

computed_base::~computed_base( void )
{
	clear_graph();
}

////////////////////////////////////////

computed_base::computed_base( const computed_base &o )
	: _graph( o._graph )
{
	set_id( o._id );
}

////////////////////////////////////////

computed_base::computed_base( computed_base &&o )
{
	adopt( std::move( o ) );
}

////////////////////////////////////////

computed_base &
computed_base::operator=( const computed_base &o )
{
	if ( &o != this )
		internal_copy( o );
	return *this;
}

////////////////////////////////////////

computed_base &
computed_base::operator=( computed_base &&o )
{
	adopt( std::move( o ) );
	return *this;
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
		if ( ! n.value().has_value() )
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
	std::unique_lock<std::mutex> lk( _graph->_value_get_mutex );
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
computed_base::new_id_notify( void *ud, node_id old, node_id nid )
{
	computed_base *cb = reinterpret_cast<computed_base *>( ud );
//	std::cout << "new_id_notify: old " << old << " new " << nid << std::endl;
	if ( cb->_id == old )
		cb->_id = nid;
}

////////////////////////////////////////

void
computed_base::set_id( node_id i )
{
	if ( _id != nullnode )
	{
		if ( _graph )
			_graph->unreference( _id, new_id_notify, this );
	}
	_id = i;
	if ( _id != nullnode )
	{
		if ( _graph )
			_graph->reference( _id, new_id_notify, this );
	}
}

////////////////////////////////////////

void
computed_base::adopt( computed_base &&o )
{
	// need to update the ref count and leave the old one since we're
	// passing this off to the graph
	clear_graph();
	_graph = o._graph;
	set_id( o._id );
}

////////////////////////////////////////

void
computed_base::internal_copy( const computed_base &o )
{
	clear_graph();
	_graph = o._graph;
	set_id( o._id );
}

////////////////////////////////////////

} // engine



