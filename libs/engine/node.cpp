// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "node.h"
#include <base/contract.h>
#include <algorithm>

////////////////////////////////////////

namespace engine
{

////////////////////////////////////////

node::node( void )
{
}

////////////////////////////////////////

node::node( op_id o, const dimensions &d, std::initializer_list<node_id> inputs, any val, hash::value hv )
	: _hash( std::move( hv ) ), _dims( d ), _value( std::move( val ) ), _op_id( o )
{
	resize_edges( static_cast<uint32_t>( inputs.size() ), 0 );
	std::copy( inputs.begin(), inputs.end(), _edges );
}

////////////////////////////////////////

node::node( op_id o, const dimensions &d, const std::vector<node_id> &inputs, any val, hash::value hv )
	: _hash( std::move( hv ) ), _dims( d ), _value( std::move( val ) ), _op_id( o )
{
	resize_edges( static_cast<uint32_t>( inputs.size() ), 0 );
	std::copy( inputs.begin(), inputs.end(), _edges );
}

////////////////////////////////////////

node::node( const node &n )
	: _hash( n._hash ),
	  _dims( n._dims ),
	  _value( n._value ),
	  _input_count( n._input_count ),
	  _output_count( n._output_count ),
	  _storage_count( n._storage_count ),
	  _op_id( n._op_id ),
	  _flags( n._flags )
{
	if ( n._edges )
	{
		_edges = new node_id[_storage_count];
		std::copy( n._edges, n._edges + _input_count + _output_count, _edges );
	}
}

////////////////////////////////////////

node &node::operator=( const node &n )
{
	if ( this != &n )
	{
		_hash = n._hash;
		_dims = n._dims;
		_value = n._value;
		_op_id = n._op_id;
		resize_edges( n._input_count, n._output_count );
		std::copy( n._edges, n._edges + _input_count + _output_count, _edges );
	}
	return *this;
}

////////////////////////////////////////

node::node( node &&n ) noexcept
	: _hash( std::move( n._hash ) ),
	_dims( n._dims ),
	_value( std::move( n._value ) ),
	_edges( n._edges ),
	_input_count( n._input_count ),
	_output_count( n._output_count ),
	_storage_count( n._storage_count ),
	_op_id( n._op_id ),
	_flags( n._flags )
{
	n._edges = nullptr;
	n._storage_count = 0;
}

////////////////////////////////////////

node &node::operator=( node &&n ) noexcept
{
	swap( n );

	return *this;
}

////////////////////////////////////////

node::~node( void )
{
	delete [] _edges;
}

////////////////////////////////////////

void
node::add_output( node_id o )
{
	uint32_t newIdx = _output_count;
	resize_edges( _input_count, _output_count + 1 );
	_edges[_input_count + newIdx] = o;
}

////////////////////////////////////////

void
node::remove_output( node_id o )
{
	if ( _output_count > 1 )
	{
		node_id *beginOuts = _edges + _input_count;
		node_id *outs = beginOuts;
		node_id *endOuts = outs + _output_count;
		while ( outs != endOuts )
		{
			if ( (*outs) == o )
				break;
			++outs;
		}
//		std::cout << this << ": searching for node " << o << " found index " << (outs-beginOuts) << std::endl;
		postcondition( outs != endOuts, "node id {0} not an output of this node", o );
		std::rotate( outs, outs + 1, endOuts );
		--_output_count;
	}
	else if ( _output_count == 1 )
	{
		precondition( output( 0 ) == o, "node id {0} not an output of this node", o );
		_output_count = 0;
	}
}

////////////////////////////////////////

void
node::update_input( node_id oldid, node_id newid )
{
	for ( uint32_t i = 0; i < _input_count; ++i )
	{
		if ( _edges[i] == oldid )
			_edges[i] = newid;
	}
}

////////////////////////////////////////

void
node::update_output( node_id oldid, node_id newid )
{
	node_id *outs = _edges + _input_count;
	for ( uint32_t i = 0; i < _output_count; ++i )
	{
		if ( outs[i] == oldid )
			outs[i] = newid;
	}
}

////////////////////////////////////////

void
node::swap( node &o )
{
	std::swap( _hash, o._hash );
	std::swap( _dims, o._dims );
	std::swap( _value, o._value );
	std::swap( _edges, o._edges );
	std::swap( _input_count, o._input_count );
	std::swap( _output_count, o._output_count );
	std::swap( _storage_count, o._storage_count );
	std::swap( _op_id, o._op_id );
	std::swap( _flags, o._flags );
}

////////////////////////////////////////

void
node::resize_edges( uint32_t num_in, uint32_t num_out )
{
	uint32_t s = num_in + num_out;
	if ( s > _storage_count )
	{
		// make sure we have room for at least 1 output
		s = num_in + std::max( num_out * 2, uint32_t(1) );
		node_id *nEdges = new node_id[s];
		if ( _edges )
		{
			std::copy( _edges, _edges + std::min(num_in, _input_count), nEdges );
			std::copy( _edges + _input_count, _edges + _input_count + std::min( _output_count, num_out ), nEdges + num_in );
			delete [] _edges;
		}
		_edges = nEdges;
		_storage_count = s;
	}
	else if ( s == 0 )
	{
		delete [] _edges;
		_edges = nullptr;
		_storage_count = 0;
	}

	_input_count = num_in;
	_output_count = num_out;
}

////////////////////////////////////////

} // engine



