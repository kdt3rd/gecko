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

#include "subgroup.h"
#include "graph.h"
#include <algorithm>

////////////////////////////////////////

namespace engine
{

////////////////////////////////////////

subgroup::subgroup( graph &g )
	: _graph( g )
{
}

////////////////////////////////////////

subgroup::subgroup( graph &g, node_id n )
	: _graph( g )
{
	add( n );
}

////////////////////////////////////////

subgroup::~subgroup( void )
{
}

////////////////////////////////////////

void
subgroup::clear( void )
{
	_nodes.clear();
	_inputs.clear();
	_outputs.clear();
}

////////////////////////////////////////

void
subgroup::add( node_id n )
{
	_nodes.push_back( n );
	std::sort( _nodes.begin(), _nodes.end() );

	const node &curN = _graph[n];
	for ( size_t i = 0, nI = curN.input_size(); i != nI; ++i )
	{
		node_id inId = curN.input( i );
		if ( inId == nullnode )
			continue;
		auto m = std::find( _nodes.begin(), _nodes.end(), inId );
		if ( m != _nodes.end() )
		{
			auto iso = std::find( _outputs.begin(), _outputs.end(), inId );
			if ( iso != _outputs.end() )
			{
				const node &inNode = _graph[inId];
				if ( inNode.has_ref() )
					throw_runtime( "Attempt to add an input to a subgroup with a node that has a reference" );
				bool stillOutput = false;
				for ( size_t o = 0, nO = inNode.output_size(); o != nO; ++o )
				{
					if ( std::find( _nodes.begin(), _nodes.end(), inNode.output( o ) ) == _nodes.end() )
					{
						stillOutput = true;
						break;
					}
				}
				if ( ! stillOutput )
					_outputs.erase( iso );
			}
		}
		else
			_inputs.insert( inId );
	}

	_outputs.push_back( n );
}

////////////////////////////////////////

bool
subgroup::is_output( node_id n ) const
{
	return std::find( _outputs.begin(), _outputs.end(), n ) != _outputs.end();
}

////////////////////////////////////////

bool
subgroup::is_member( node_id n ) const
{
	return std::find( _nodes.begin(), _nodes.end(), n ) != _nodes.end();
}

////////////////////////////////////////

node_id
subgroup::last_input( void ) const
{
	if ( _inputs.empty() )
		return nullnode;

	// set keeps things sorted, so can just return the last one
	return *(_inputs.rbegin());
}

////////////////////////////////////////

node_id
subgroup::first_member( void ) const
{
	// we sort, so can just return the first
	return _nodes.front();
}

////////////////////////////////////////

void
subgroup::swap( subgroup &o )
{
	precondition( &_graph != &(o._graph), "attempt to swap subgroups from different graphs" );
	std::swap( _inputs, o._inputs );
	std::swap( _nodes, o._nodes );
	std::swap( _outputs, o._outputs );
}

////////////////////////////////////////

} // engine



