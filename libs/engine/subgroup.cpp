//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include "subgroup.h"
#include "graph.h"
#include <algorithm>
#include <base/thread_util.h>

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

	auto rmInTag = _inputs.find( n );
	if ( rmInTag != _inputs.end() )
		_inputs.erase( rmInTag );

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

	size_t nOut = curN.output_size();
	bool isOut = nOut == 0;
	for ( size_t o = 0; o != nOut; ++o )
	{
		if ( ! is_member( curN.output( o ) ) )
		{
			isOut = true;
			break;
		}
	}
	if ( isOut )
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

bool
subgroup::can_merge( const subgroup &o ) const
{
	if ( internal_merge_check( o ) )
		return o.internal_merge_check( *this );
	return false;
}

////////////////////////////////////////

bool
subgroup::internal_merge_check( const subgroup &o ) const
{
	graph &g = _graph;
	const registry &ops = g.op_registry();

	for ( auto &n: _nodes )
	{
		node &cur = g[n];
		if ( ops[cur.op()].processing_style() == op::style::N_TO_ONE )
		{
			for ( auto i = cur.begin_inputs(), ei = cur.end_inputs(); i != ei; ++i )
			{
				if ( o.is_member( *i ) )
				{
					std::cout << "internal_merge_check fails because input " << i << " for node " << n << " is a member of other" << std::endl;
					return false;
				}
				if ( g.has_ancestor( *i, o.members() ) )
				{
					std::cout << "internal_merge_check fails because input " << i << " for node " << n << " has an ancestor in other" << std::endl;
					return false;
				}
			}
		}
	}
	return true;
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

void
subgroup::process( void )
{
	precondition( ! _outputs.empty(), "empty subgroup being processed" );

	graph &g = _graph;
	const registry &ops = g.op_registry();

	for ( auto n: _outputs )
	{
		node &cur = g[n];
		cur.value() = ops[cur.op()].function().create_value( cur.dims() );
	}

	node &f = g[_outputs.front()];

	// how to make sure all inputs are computed prior to this?
	//    ->
	// rotate first subgroup item to after last input (or when
	// copying / merging / adding, insert the input prior to the
	// first subgroup item)
	ops[f.op()].function().dispatch_group( *this, f.dims() );
	
	_processed = true;
}

////////////////////////////////////////

void
subgroup::bind_functions( std::vector<std::shared_ptr<subgroup_function>> &funcs )
{
	funcs.reserve( _nodes.size() );
	graph &g = _graph;
	const registry &ops = g.op_registry();
	size_t outI = 0;
	for ( auto n: _nodes )
	{
		funcs.emplace_back( ops[g[n].op()].function().create_group_function() );
		funcs.back()->bind( funcs, *this, g[n] );
		if ( is_output( n ) )
			funcs.back()->output_index( outI++ );
	}
}

////////////////////////////////////////

any &
subgroup::output_val( size_t i )
{
	return _graph[_outputs[i]].value();
}

////////////////////////////////////////

size_t
subgroup::func_idx( node_id n )
{
	size_t idx = 0;
	size_t N = _nodes.size();

	for ( ; idx != N; ++idx )
	{
		if ( _nodes[idx] == n )
			break;
	}
	postcondition( idx != N, "request for node {0} that is not a member of subgroup with first member {1}", n, first_member() );

	return idx;
}

////////////////////////////////////////

} // engine



