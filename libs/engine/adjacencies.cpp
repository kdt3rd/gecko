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

#include "adjacencies.h"
#include <algorithm>
#include <base/contract.h>
#include <limits>

////////////////////////////////////////

namespace engine
{

////////////////////////////////////////

adjacencies::adjacencies( void )
{
}

////////////////////////////////////////

adjacencies::~adjacencies( void )
{
	delete [] _input_edges;
}

////////////////////////////////////////

adjacencies::adjacencies( std::initializer_list<node_id> inputs )
{
	resize( inputs.size() );
	node_id *ins = _input_edges;
	for ( node_id v: inputs )
		*ins++ = v;
}

////////////////////////////////////////

adjacencies::adjacencies( adjacencies &&a ) noexcept
{
	std::swap( _input_edges, a._input_edges );
	std::swap( _output_edges, a._output_edges );
	std::swap( _storage_count, a._storage_count );
	std::swap( _output_count, a._output_count );
}

////////////////////////////////////////

adjacencies &adjacencies::operator=( adjacencies &&a ) noexcept
{
	clear();
	std::swap( _input_edges, a._input_edges );
	std::swap( _output_edges, a._output_edges );
	std::swap( _storage_count, a._storage_count );
	std::swap( _output_count, a._output_count );
}

////////////////////////////////////////

adjacencies::adjacencies( const adjacencies &o )
{
	resize( o.input_size(), o.output_size() );
	std::copy( o.begin_inputs(), o.end_inputs(), _input_edges );
	std::copy( o.begin_outputs(), o.end_outputs(), _output_edges );
}

////////////////////////////////////////

adjacencies &adjacencies::operator=( const adjacencies &o )
{
	if ( &o != this )
	{
		resize( o.input_size(), o.output_size() );
		std::copy( o.begin_inputs(), o.end_inputs(), _input_edges );
		std::copy( o.begin_outputs(), o.end_outputs(), _output_edges );
	}
	return *this;
}

////////////////////////////////////////

void
adjacencies::add_output( node_id o )
{
	size_t i = output_size();
	resize( input_size(), i + 1 );
	output( i ) = o;
}


////////////////////////////////////////

void adjacencies::clear( void )
{
	delete [] _input_edges;
	_input_edges = nullptr;
	_output_edges = nullptr;
	_storage_count = 0;
	_output_count = 0;
}

////////////////////////////////////////

void adjacencies::resize( size_t num_in, size_t num_out )
{
	size_t newCount = num_in + num_out;
	if ( newCount > _storage_count )
	{
		size_t numAlloc = num_in + num_out * 2;
		precondition( numAlloc < std::numeric_limits<uint32_t>::max(), "More inputs / outputs than adjacencies structure permits" );

		node_id *nmem = new node_id[numAlloc];
		if ( _input_edges )
		{
			std::copy( _input_edges, _input_edges + std::min( num_in, input_size() ), nmem );
			if ( _output_count > 0 )
				std::copy( _output_edges, std::min( num_out, _output_count ), nmem + num_in );
			delete [] _input_edges;
		}
		_input_edges = nmem;
		_output_edges = nmem + num_in;
		_output_count = static_cast<uint32_t>( num_out );
		_storage_count = static_cast<uint32_t>( numAlloc );
	}
}

////////////////////////////////////////

} // engine



