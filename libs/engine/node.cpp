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
	: _hash( std::move( hv ) ), _value( std::move( val ) ),
	  _dims( d ), _op_id( o )
{
	resize_edges( inputs.size(), 0 );
	std::copy( inputs.begin(), inputs.end(), _edges );
}

////////////////////////////////////////

node::node( op_id o, const dimensions &d, const std::vector<node_id> &inputs, any val, hash::value hv )
	: _hash( std::move( hv ) ), _value( std::move( val ) ),
	  _dims( d ), _op_id( o )
{
	resize_edges( inputs.size(), 0 );
	std::copy( inputs.begin(), inputs.end(), _edges );
}

////////////////////////////////////////

node::node( const node &n )
	: _hash( n._hash ),
	  _value( n._value ),
	  _dims( n._dims ),
	  _output_count( n._output_count ),
	  _storage_count( n._storage_count ),
	  _op_id( n._op_id ),
	  _input_count( n._input_count ),
	  _flags( n._flags ),
	  _exec_time( n._exec_time )
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
		_value = n._value;
		_dims = n._dims;
		_op_id = n._op_id;
		resize_edges( n._input_count, n._output_count );
		std::copy( n._edges, n._edges + _input_count + _output_count, _edges );
	}
	return *this;
}

////////////////////////////////////////

node::node( node &&n ) noexcept
	: _hash( std::move( n._hash ) ),
	_value( std::move( n._value ) ),
	_dims( n._dims ), _edges( n._edges ),
	_output_count( n._output_count ),
	_storage_count( n._storage_count ),
	_op_id( n._op_id ),
	_input_count( n._input_count ),
	_flags( n._flags ),
	_exec_time( n._exec_time )
{
	n._edges = nullptr;
	n._storage_count = 0;
}

////////////////////////////////////////

node &node::operator=( node &&n ) noexcept
{
	std::swap( _hash, n._hash );
	_value = std::move( n._value );
	std::swap( _dims, n._dims );
	std::swap( _edges, n._edges );
	std::swap( _output_count, n._output_count );
	std::swap( _storage_count, n._storage_count );
	std::swap( _op_id, n._op_id );
	std::swap( _input_count, n._input_count );
	std::swap( _flags, n._flags );
	std::swap( _exec_time, n._exec_time );

	return *this;
}

////////////////////////////////////////

node::~node( void )
{
	delete _edges;
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
		postcondition( outs != endOuts, "node id {0} not an output of this node", o );
		std::rotate( outs, outs + 1, endOuts );
		--_output_count;
	}
	else
	{
		precondition( output( 0 ) == o, "node id {0} not an output of this node", o );
		_output_count = 0;
	}
}

////////////////////////////////////////

void
node::resize_edges( uint8_t num_in, uint32_t num_out )
{
	uint32_t s = static_cast<uint32_t>( num_in ) + num_out;
	if ( s > _storage_count )
	{
		// make sure we have room for at least 1 output
		s = static_cast<uint32_t>( num_in ) + std::max( num_out * 2, uint32_t(1) );
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
		delete _edges;
		_edges = nullptr;
		_storage_count = 0;
	}

	_input_count = num_in;
	_output_count = num_out;
}

////////////////////////////////////////

} // engine



