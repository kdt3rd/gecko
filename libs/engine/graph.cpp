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

#include "graph.h"

#include <vector>
#include <map>
#include <set>
#include <deque>
#include <stack>
#include <algorithm>
#include <numeric>
#include <base/contract.h>
#include <base/scope_guard.h>
#include <base/compiler_abi.h>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "registry.h"

////////////////////////////////////////

namespace engine
{

////////////////////////////////////////

std::ostream &
operator<<( std::ostream &os, const dimensions &d )
{
	os << "[ " << d.x << ", " << d.y << ", " << d.z << ", " << d.w << " ]";
	return os;
}

////////////////////////////////////////
graph::graph( const registry &r )
	: _ops( r )
{
}

////////////////////////////////////////

graph::~graph( void )
{
}

////////////////////////////////////////

node_id
graph::add_node( const base::cstring &opname, const dimensions &d, std::initializer_list<node_id> inputs )
{
	return add_node( _ops.find( opname ), any(), d, inputs );
}

////////////////////////////////////////

void
graph::tag_rvalue( node_id n )
{
	precondition( n < _nodes.size(), "invalid node id {0}", n );
	_nodes[n].set_rvalue();
}

////////////////////////////////////////

const any &
graph::get_value( node_id n )
{
	precondition( n < _nodes.size(), "invalid node id {0}", n );
	node &node = _nodes[n];
	any &v = node.value();
	if ( v.empty() )
		return process( n );

	return v;
}

////////////////////////////////////////

node_id
graph::copy_node( const graph &o, node_id n )
{
	precondition( n < o.size(), "invalid node id {0}", n );
	if ( &o == this )
		return n;

	std::vector<node_id> tocopy;
	std::map<node_id, node_id> nodemap;
	tocopy.push_back( n );
	std::vector<node_id> inputs;

	while ( ! tocopy.empty() )
	{
		const node &cur = o[tocopy.back()];
		bool doAdd = true;
		size_t nInputs = cur.input_size();
		inputs.resize( nInputs );

		for ( size_t i = 0; i != nInputs; ++i )
		{
			node_id curIn = cur.input( i );
			if ( curIn == nullnode )
			{
				inputs[i] = nullnode;
				continue;
			}

			auto inN = nodemap.find( curIn );
			if ( inN == nodemap.end() )
			{
				doAdd = false;
				tocopy.push_back( curIn );
			}
			else
				inputs[i] = inN->second;
		}

		if ( doAdd )
		{
			const registry &srcReg = o.op_registry();
			op_id opid = cur.op();
			node_id nnode;

			// if the registries are the same, we don't have to re-look things up
			if ( &srcReg != &_ops )
			{
				hash h;
				if ( ! cur.value().empty() )
					cur.value().binary_stream( h );

				opid = _ops.find( srcReg[opid].name() );
				nnode = add_node( opid, cur.value(), cur.dims(), inputs, h );
			}
			else
				nnode = add_node( opid, cur.value(), cur.dims(), inputs, cur.hash_value() );

			nodemap[tocopy.back()] = nnode;
//			std::cout << "graph " << this << ": adding op during copy: " << cur.op() << " hash " << cur.hash_value() << " --> " << nnode << " hash " << _nodes[nnode].hash_value() << std::endl;
			
			tocopy.pop_back();
		}
	}

	return nodemap[n];
}

////////////////////////////////////////

node_id
graph::move_node( graph &o, node_id n )
{
	precondition( n < o.size(), "invalid node id {0}", n );
	if ( &o == this )
		return n;
	precondition( o[n].output_size() == 0, "Attempt to move a node with outputs in graph" );

	node_id nnode = copy_node( o, n );
	o.remove_node( n );
	return nnode;
}

////////////////////////////////////////

void
graph::remove_node( node_id n )
{
	precondition( n < size(), "invalid node id {0}", n );

	std::deque<node_id> del;
	while ( true )
	{
		node &curDelN = _nodes[n];
		const node_id *b = curDelN.begin_inputs();
		const node_id *be = curDelN.end_inputs();
		while ( b != be )
		{
			if ( *b == nullnode )
			{
				++b;
				continue;
			}

			node &curIn = _nodes[*b];
			if ( curIn.is_rvalue() )
				del.push_back( *b );
			else if ( curIn.output_size() == 1 && ! curIn.has_ref() )
			{
				// no references, let's schedule it for deletion
				del.push_back( *b );
			}
//			std::cout << "remove node " << n << " from outputs of " << *b << std::endl;
			curIn.remove_output( n );
			++b;
		}
		curDelN = node();

		if ( del.empty() )
			break;
		n = del.front();
		del.pop_front();
	}
}

////////////////////////////////////////

void
graph::clean_graph( void )
{
	clear_grouping();

	std::set<node_id> toDel;
	bool didAdd = true;
	while ( didAdd )
	{
		didAdd = false;
		for ( node_id nid = 0, N = static_cast<node_id>( _nodes.size() ); nid != N; ++nid )
		{
			if ( toDel.find( nid ) != toDel.end() )
				continue;

			node &n = _nodes[nid];
			// cleared node
			if ( n.op() == nullop )
			{
				toDel.insert( nid );
				didAdd = true;
				continue;
			}

			// if we have a value, clean up any remaining inputs as a first step
			bool hasValue = ! n.value().empty();
			if ( hasValue )
			{
				size_t nInputs = n.input_size();
				for ( size_t i = 0; i != nInputs; ++i )
				{
					node_id &inid = n.input( i );
					if ( inid != nullnode )
					{
						node &curIn = _nodes[inid];
						curIn.remove_output( nid );
						if ( curIn.output_size() == 0 && ! curIn.has_ref() )
							toDel.insert( inid );
						inid = nullnode;
					}
				}
			}

			// if there's a reference, skip
			if ( n.has_ref() )
				continue;

			size_t validOuts = 0;
			for ( size_t o = 0, nO = n.output_size(); o != nO; ++o )
			{
				node_id onid = n.output( o );
				if ( _nodes[onid].value().empty() && toDel.find( onid ) == toDel.end() )
					++validOuts;
			}

			if ( validOuts == 0 )
			{
				toDel.insert( nid );
				didAdd = true;
				continue;
			}
		}
	}

	if ( toDel.empty() )
		return;

//	std::cout << "removing " << toDel.size() << " nodes" << std::endl;
	std::map<node_id, node_id> newnodemap;
	std::vector<node> nnodes;
	nnodes.reserve( _nodes.size() );
	for ( node_id nid = 0, N = static_cast<node_id>( _nodes.size() ); nid != N; ++nid )
	{
		if ( toDel.find( nid ) == toDel.end() )
		{
			const node &n = _nodes[nid];
			node newn = n;
			for ( size_t i = 0, nI = n.input_size(); i != nI; ++i )
			{
				auto nn = newnodemap.find( n.input( i ) );
				if ( nn == newnodemap.end() )
					newn.input( i ) = nullnode;
				else
					newn.input( i ) = nn->second;
			}
			newnodemap[nid] = static_cast<node_id>( nnodes.size() );
			nnodes.emplace_back( std::move( newn ) );
		}
	}

	// now that we've added everything, update the outputs and update the hash-to-node map
	std::map<hash::value, node_id> nh2nmap;
	for ( node_id nid = 0, N = static_cast<node_id>( nnodes.size() ); nid != N; ++nid )
	{
		node &n = nnodes[nid];
		for ( size_t o = 0; o != n.output_size(); ++o )
		{
			node_id oldout = n.output( o );
			auto nn = newnodemap.find( oldout );
			if ( nn == newnodemap.end() )
			{
//				std::cout << "removing output node " << oldout << " from " << nid << std::endl;
				--o;
				n.remove_output( oldout );
			}
			else
				n.output( o ) = nn->second;
		}
		nh2nmap[n.hash_value()] = nid;
	}

	// notify our references of the new node ids
	std::map<node_id, reference_list> nrc;
	for ( auto &x: _ref_counts )
	{
		node_id oldid = x.first;
		auto nn = newnodemap.find( oldid );
		if ( nn == newnodemap.end() )
			throw_runtime( "Killed node {0} which has a reference", oldid );
		node_id newid = nn->second;
		for ( auto &ref: x.second )
			ref.first( ref.second, oldid, newid );
		std::swap( nrc[newid], x.second );
	}
	// finally, update with all our changes
	std::swap( _nodes, nnodes );
	std::swap( _ref_counts, nrc );
	std::swap( _hash_to_node, nh2nmap );
}

////////////////////////////////////////

static inline std::string esc_dot( const std::string &x )
{
	return base::replace( x, { { '<', "\\<" }, { '>', "\\>" }, { '{', "\\{" }, { '}', "\\}" }, { '|', "\\|" } } );
}

////////////////////////////////////////

static void emit_node( std::ostream &os, int indent, const registry &ops, node_id n, const node &curN, bool incHash, bool isSGoutput = false )
{
	os << std::setw( indent ) << std::setfill( ' ' ) << "" << 'N' << n << " [label=\"";
	if ( curN.input_size() > 0 )
	{
		os << "{{";
		for ( size_t i = 0, nI = curN.input_size(); i != nI; ++i )
		{
			if ( i > 0 )
				os << '|';
			if ( curN.input( i ) == nullnode )
				os << "<i" << i << ">NIL";
			else
				os << "<i" << i << ">" << esc_dot( base::demangle( ops[curN.op()].input_type( i ) ) );
		}
		os << "}|N " << n << "\\n" << esc_dot( ops[curN.op()].name() );
		if ( incHash )
			os << "\\n" << curN.hash_value();
		os << '}';
	}
	else
	{
		os << "N " << n;
		if ( ! curN.value().empty() )
		{
			const engine::any &v = curN.value();
			if ( v.is_type<float>() )
				os << "\\nfloat " << base::any_cast<float>( v );
			else if ( v.is_type<int>() )
				os << "\\nint " << base::any_cast<int>( v );
			else if ( v.is_type<int64_t>() )
				os << "\\nint64_t " << base::any_cast<int64_t>( v );
			else if ( v.is_type<double>() )
				os << "\\ndouble " << base::any_cast<double>( v );
			else
				os << "\\n" << esc_dot( ops[curN.op()].name() ) << " (" << esc_dot( base::demangle( ops[curN.op()].function().result_type() ) );
		}
		else
			os << "\\n" << esc_dot( ops[curN.op()].name() );
		if ( incHash )
			os << "\\n" << curN.hash_value();
	}
	os << '\"';
	if ( isSGoutput )
		os << ", style=filled, fillcolor=\"#DDFFFF\"";
	else if ( curN.has_ref() )
		os << ", style=filled, fillcolor=\"#DDDDFF\"";
	else if ( curN.output_size() == 0 )
		os << ", style=filled, fillcolor=\"#FFDDDD\"";
	else if ( ! curN.value().empty() )
		os << ", style=filled, fillcolor=\"#DDFFDD\"";

	os << "];\n";
}

////////////////////////////////////////

void
graph::dump_dot( const std::string &fn, bool incHash ) const
{
	std::ofstream gdot( fn );
	dump_dot( gdot, incHash );
	std::cout << "Saved graph to '" << fn << "'" << std::endl;
}

////////////////////////////////////////

void
graph::dump_dot( std::ostream &os, bool incHash ) const
{
	os << "digraph graph_" << this <<
		" {\n"
	    "  graph [label=\"green means has value\\nblue means has reference\\nred means dangling no output\"];\n"
	    "  node [shape=record];\n"
	    "  edge [style=solid,arrowhead=normal,arrowtail=none];\n"
		"\n";

	std::set<node_id> didEmitNode;
	std::set<std::pair<node_id, node_id>> didEmitEdge;
	size_t sgnum = 1;
	for ( auto &s: _subgroups )
	{
		if ( s.empty() )
			continue;

		os << "  subgraph cluster_" << sgnum << "{\n";
		for ( node_id n: s.members() )
		{
			emit_node( os, 4, _ops, n, _nodes[n], incHash, s.is_output( n ) );
			didEmitNode.insert( n );
		}

		// emit internal edges
		for ( node_id n: s.members() )
		{
			const node &curN = _nodes[n];
			for ( size_t i = 0, nI = curN.input_size(); i != nI; ++i )
			{
				node_id inN = curN.input( i );
				if ( s.is_member( inN ) )
				{
					os << "    N" << inN << " -> N" << n << ":i" << i << ";\n";
					didEmitEdge.insert( std::make_pair( inN, n ) );
				}
			}
		}

		os << "\n    label=\"subgroup " << sgnum << " outputs " << s.outputs().size() << "\";\n    color=black\n  }\n";
		++sgnum;
	}

	for ( node_id n = 0, N = static_cast<node_id>( _nodes.size() ); n != N; ++n )
	{
		const node &curN = _nodes[n];
		if ( curN.op() == nullop )
			continue;

		if ( didEmitNode.find( n ) != didEmitNode.end() )
			continue;

		emit_node( os, 2, _ops, n, curN, incHash );
	}

	// done all the nodes, now do the edges
	for ( size_t n = 0, N = _nodes.size(); n != N; ++n )
	{
		const node &curN = _nodes[n];
		if ( curN.op() == nullop )
			continue;

		for ( size_t i = 0, nI = curN.input_size(); i != nI; ++i )
		{
			node_id inN = curN.input( i );
			if ( inN == nullnode )
				continue;

			std::pair<node_id, node_id> p = std::make_pair( inN, n );
			if ( didEmitEdge.find( p ) == didEmitEdge.end() )
				os << "  N" << curN.input( i ) << " -> N" << n << ":i" << i << ";\n";
		}
	}
	
	os << "\n}\n";
}

////////////////////////////////////////

void
graph::dump_refs( std::ostream &os ) const
{
	if ( _ref_counts.empty() )
		return;

	os << "reference counts:\n"
	   << "  node    count\n"
	   << "--------  -----\n";
	for ( auto &r: _ref_counts )
		os << std::setw( 8 ) << std::setfill( ' ' ) << r.first << "  " << std::setw( 5 ) << r.second.size() << std::endl;
}

////////////////////////////////////////

static void update_nid( void *ud, node_id old, node_id nid )
{
	node_id *nptr = reinterpret_cast<node_id *>( ud );
	*nptr = nid;
}

////////////////////////////////////////

const any &
graph::process( node_id nid )
{
//	std::cout << "Request to process node " << nid << " (op " << _ops[n.op()].name() << ")" << std::endl;

	// stash a reference so we can correctly get the  value on exit
	reference( nid, update_nid, &nid );
	on_scope_exit{ unreference( nid, update_nid, &nid ); };
	
	optimize();
//	std::cout << "optimized, start of processing: " << _start_of_processing << std::endl;

	// build the list of unprocessed nodes connected to this node
	_process_list.clear();
	std::deque<node_id> check;
	check.push_back( nid );
	while ( ! check.empty() )
	{
		node_id cur = check.front();
		check.pop_front();
		_process_list.insert( cur );

		const node &curN = _nodes[cur];
		size_t nInputs = curN.input_size();
		for ( size_t i = 0; i != nInputs; ++i )
		{
			node_id in = curN.input( i );
			precondition( in != nullnode, "input prematurely cleaned" );
			if ( ! _nodes[in].value().empty() )
				continue;

			if ( _process_list.find( in ) == _process_list.end() )
				check.push_back( in );
		}
	}

	std::vector<any> inputs;

	std::cout << "Have " << _process_list.size() << " nodes to process" << std::endl;
	for ( node_id c: _process_list )
	{
		node &curN = _nodes[c];
		if ( curN.op() == nullop )
			continue;
		if ( ! curN.value().empty() )
			continue;

//		std::cout << "  computing node " << c << std::endl;
		if ( curN.in_subgroup() )
		{
			size_t sgi = _node_to_subgroup[c];
			subgroup &sg = _subgroups[sgi];
//			std::cout << "   -> processing subgroup " << sgi << std::endl;
			if ( sg.processed() )
			{
//				std::cout << "       -> subgroup already processed" << std::endl;
				continue;
			}
			sg.process();
			continue;
		}

		size_t nInputs = curN.input_size();
		inputs.resize( nInputs );
		for ( size_t i = 0; i != nInputs; ++i )
		{
			node_id inid = curN.input( i );
			node &curIn = _nodes[inid];
			inputs[i] = curIn.value();
		}

		const op &o = _ops[curN.op()];
		curN.value() = o.function().process( *this, curN.dims(), inputs );
	}

	clear_grouping();
	clean_graph();

	std::cout << "Final nid " << nid << " with " << _nodes.size() << " nodes left in graph" << std::endl;
	return _nodes[nid].value();
}

////////////////////////////////////////

void
graph::optimize( void )
{
	_start_of_processing = 0;
	clean_graph();
	clear_grouping();

	for ( size_t n = 0, N = _nodes.size(); n != N; ++n )
	{
		node &cur = _nodes[n];
		// only 1 output and no remaining references in
		// computed_value, we can tag as an rvalue
		if ( cur.output_size() == 1 && ! cur.has_ref() )
			cur.set_rvalue();
	}

	move_constants();
	apply_peephole();
	apply_grouping();

	if ( ! _subgroups.empty() )
	{
		// now need to re-order any group's inputs to before the first node in the group
		std::vector<node_id> neworder( _nodes.size(), nullnode );

		bool made_change = true;
		while ( made_change )
		{
			made_change = false;
			std::iota( neworder.begin(), neworder.end(), 0 );

			for ( auto &sg: _subgroups )
			{
				node_id lastIn = sg.last_input();
				if ( lastIn == nullnode )
					continue;

				node_id firstMember = sg.first_member();
				if ( firstMember < lastIn )
				{
//					std::cout << "Need to move inputs around first member " << firstMember << " last input " << lastIn << std::endl;
					made_change = true;
					node_id nnode = lastIn + 1;
					for ( node_id member: sg.members() )
					{
//						std::cout << " member: " << member << " -> nnode " << nnode << std::endl;
						neworder[member] = nnode;
						++nnode;
					}
				}
			}
			if ( made_change )
			{
				clear_grouping();
//				std::cout << "New grouping-based ordering:\n";
//				for ( size_t i = 0, N = neworder.size(); i != N; ++i )
//					std::cout << i << ": " << neworder[i] << '\n';
//				std::cout << std::endl;
				made_change = false;
				for ( size_t i = 0, N = neworder.size(); i != N; ++i )
				{
					if ( static_cast<size_t>( neworder[i] ) > i )
					{
						node_id newdest = neworder[i];
						node_id curP = static_cast<node_id>( i );
						for ( size_t j = i + 1; j != N; ++j )
						{
							if ( neworder[j] < newdest )
							{
//								static int rotate_in = 1;
								made_change = true;
//								std::stringstream fn;
//								fn << "rotate_order_" << rotate_in << ".dot";
//								++rotate_in;
//								std::cout << "   -> " << j << " rotate " << neworder[j] << " to " << curP << " newdest " << newdest << std::endl;
								rotate_node( neworder[j], curP );
//								dump_dot( fn.str() );
								++curP;
							}
							if ( j >= newdest )
								break;
						}
					}
					if ( made_change )
						break;
				}
				if ( made_change )
				{
					apply_grouping();
//					dump_dot( "rotate_graph.dot" );
				}
			}
		}
	}
}

////////////////////////////////////////

void
graph::move_constants( void )
{
	node_id curConstantPos = 0;
	node_id curPos = 0;
	node_id N = static_cast<node_id>( _nodes.size() );
	for ( ; curPos != N; ++curPos )
	{
		if ( _nodes[curPos].value().empty() )
			break;
		++curConstantPos;
	}
	for ( ; curPos != N; ++curPos )
	{
		if ( _nodes[curPos].value().empty() )
			continue;

		if ( curPos != curConstantPos )
		{
//			std::cout << "moving constant at " << curPos << " to " << curConstantPos << std::endl;
			rotate_node( curPos, curConstantPos );
		}

		++curConstantPos;
	}
	_start_of_processing = curConstantPos;
}

////////////////////////////////////////

void
graph::apply_peephole( void )
{
}

////////////////////////////////////////

void
graph::apply_grouping( void )
{
	for ( node_id n = 0, N = static_cast<node_id>( _nodes.size() ); n != N; ++n )
	{
		node &cur = _nodes[n];
		// skip dead nodes
		if ( cur.op() == nullop )
			continue;
		// and nodes with values
		if ( ! cur.value().empty() )
			continue;

		const op &curOp = _ops[cur.op()];
		switch ( curOp.processing_style() )
		{
			case op::style::ONE_TO_ONE:
			{
				size_t subI = size_t(-1);
				for ( size_t i = 0, nI = cur.input_size(); i != nI; ++i )
				{
					node_id curIn = cur.input( i );
					node &curInN = _nodes[curIn];

					if ( curInN.in_subgroup() && ! curInN.has_ref() )
					{
						// if the result type doesn't match, don't group
						if ( curOp.function().result_type() != _ops[curInN.op()].function().result_type() )
						{
							std::cout << "Skipping grouping " << n << " with " << curIn << " because function result type doesn't match" << std::endl;
							continue;
						}

						if ( curInN.dims() != cur.dims() )
						{
							std::cout << "Skipping grouping " << n << " with " << curIn << " because dimensions are different" << std::endl;
							continue;
						}

						size_t cIdx = find_subgroup( curIn );
						if ( subI == size_t(-1) )
						{
							if ( curInN.output_size() == 1 )
							{
								_subgroups[cIdx].add( n );
								_node_to_subgroup[n] = cIdx;
								cur.set_in_subgroup();
								subI = cIdx;
							}
							else
							{
								std::cout << "TODO: Need to add search such that we can pull multiple outputs into the same subgroup" << std::endl;
							}
						}
						else if ( cIdx != subI )
						{
							subI = merge_subgroups( subI, cIdx );
							_subgroups[subI].add( n );
							_node_to_subgroup[n] = subI;
							cur.set_in_subgroup();
							break;
						}
					}
				}
				if ( subI == size_t(-1) )
				{
					_node_to_subgroup[n] = _subgroups.size();
					cur.set_in_subgroup();
					_subgroups.emplace_back( subgroup( *this, n ) );
				}
				break;
			}
			case op::style::N_TO_ONE:
				_node_to_subgroup[n] = _subgroups.size();
				cur.set_in_subgroup();
				_subgroups.emplace_back( subgroup( *this, n ) );
				break;
			case op::style::MULTI_THREADED:
			case op::style::SINGLE_THREADED:
			case op::style::SOLITARY:
			case op::style::SIMPLE:
			case op::style::VALUE:
				for ( size_t i = 0, nI = cur.input_size(); i != nI; ++i )
				{
					node_id curIn = cur.input( i );
					if ( _nodes[curIn].in_subgroup() )
					{
						split_subgroup( find_subgroup( curIn ), curIn );
					}
				}
				break;
		}
	}
}

////////////////////////////////////////

void
graph::clear_grouping( void )
{
	_subgroups.clear();
	_node_to_subgroup.clear();
	for ( node_id n = 0, N = static_cast<node_id>( _nodes.size() ); n != N; ++n )
		_nodes[n].clear_in_subgroup();
}

////////////////////////////////////////

size_t
graph::find_subgroup( node_id n ) const
{
	auto x = _node_to_subgroup.find( n );
	if ( x == _node_to_subgroup.end() )
		return size_t(-1);
	return x->second;
}

////////////////////////////////////////

size_t
graph::merge_subgroups( size_t a, size_t b )
{
	size_t ret = a;
	size_t from = b;
	if ( _subgroups[a].size() < _subgroups[b].size() )
	{
		ret = b;
		from = a;
	}

	for ( auto f: _subgroups[from].members() )
	{
		_subgroups[ret].add( f );
		_node_to_subgroup[f] = ret;
	}
	_subgroups[from].clear();

	return ret;
}

////////////////////////////////////////

void
graph::split_subgroup( size_t i, node_id n )
{
	subgroup &cur = _subgroups[i];

	if ( cur.is_output( n ) )
		return;

	subgroup nI( *this );
	size_t newIdx = _subgroups.size();
	_subgroups.emplace_back( subgroup( *this ) );
	subgroup &nAfter = _subgroups.back();
	for ( node_id x: cur.members() )
	{
		if ( x <= n )
			nI.add( x );
		else
		{
			nAfter.add( x );
			_node_to_subgroup[x] = newIdx;
		}
	}
	cur.swap( nI );
}

////////////////////////////////////////

void
graph::rotate_node( node_id oldpos, node_id newpos )
{
	precondition( oldpos < _nodes.size() && newpos < _nodes.size(), "invalid positions given to rotate" );
	precondition( _nodes[oldpos].op() != nullop, "invalid node given to rotate" );

	if ( oldpos == newpos )
		return;

	node tmpStore;
	std::swap( _nodes[oldpos], tmpStore );

	int dir = 1;
	if ( newpos < oldpos )
		dir = -1;

	const node_id tmpPos = nullnode - 1;
	// put the temporary node at a temporary position and then update
	// after
	for ( size_t i = 0, nC = tmpStore.output_size(); i != nC; ++i )
	{
		node_id &outN = tmpStore.output(i);
		precondition( newpos < outN, "attempt to move a node {0} past a node {1} who has it as an input", oldpos, outN );

		_nodes[outN].update_input( oldpos, tmpPos );
	}
	for ( size_t i = 0, nC = tmpStore.input_size(); i != nC; ++i )
	{
		node_id &inN = tmpStore.input(i);
		if ( inN == nullnode )
			continue;

		precondition( newpos > inN, "attempt to move a node {0} before one of it's inputs {1}", oldpos, inN );

		_nodes[inN].update_output( oldpos, tmpPos );
	}

	node_id cur = oldpos;
	while ( cur != newpos )
	{
		node_id other = static_cast<node_id>( int64_t(cur) + dir );
		std::swap( _nodes[other], _nodes[cur] );
		node &curN = _nodes[cur];
		for ( size_t i = 0, nC = curN.output_size(); i != nC; ++i )
			_nodes[curN.output(i)].update_input( other, cur );
		for ( size_t i = 0, nC = curN.input_size(); i != nC; ++i )
		{
			node_id inN = curN.input( i );
			if ( inN == nullnode )
				continue;
			_nodes[inN].update_output( other, cur );
		}
		cur = static_cast<node_id>( int64_t(cur) + dir );
	}

	std::swap( _nodes[newpos], tmpStore );
	node &nnode = _nodes[newpos];
	// patch up final position
	for ( size_t i = 0, nC = nnode.output_size(); i != nC; ++i )
		_nodes[nnode.output(i)].update_input( tmpPos, newpos );
	for ( size_t i = 0, nC = nnode.input_size(); i != nC; ++i )
	{
		node_id inN = nnode.input( i );
		if ( inN == nullnode )
			continue;
		_nodes[inN].update_output( tmpPos, newpos );
	}
}

////////////////////////////////////////

node_id
graph::find_node( const hash::value &hv )
{
//	std::cout << "searching for hv " << hv << std::endl;
	auto i = _hash_to_node.find( hv );
	if ( i != _hash_to_node.end() )
		return i->second;
	return nullnode;
}

////////////////////////////////////////

node_id
graph::add_node( op_id op, any value, const dimensions &d, std::initializer_list<node_id> inputs )
{
	hash h;
	return add_node( op, std::move( value ), d, inputs, h );
}

////////////////////////////////////////

node_id
graph::add_node( op_id op, any value, const dimensions &d, std::initializer_list<node_id> inputs, hash &h )
{
	if ( _ops[op].input_size() != inputs.size() )
		throw_logic( "Invalid number of inputs passed to operator {0}, expect {1}, got {2}", _ops[op].name(), _ops[op].input_size(), inputs.size() );
	
	h << op << d;
	for ( auto n: inputs )
	{
		precondition( n < _nodes.size(), "Invalid input given for new node" );
		h << _nodes[n].hash_value();
	}
	hash::value hv = h.final();

//	std::cout << "graph " << this << ": Adding op " << _ops[op].name() << ": " << hv << std::endl;
	return add_node( op, std::move( value ), d, inputs, hv );
}

////////////////////////////////////////

node_id
graph::add_node( op_id op, any value, const dimensions &d, std::initializer_list<node_id> inputs, const hash::value &hv )
{
	node_id n = find_node( hv );
	if ( n != nullnode )
	{
		// simple collision check
		const node &colCheck = _nodes[n];
		if ( colCheck.op() != op || colCheck.input_size() != inputs.size() || colCheck.dims() != d )
			throw_logic( "Hash collision with existing node {0} adding opname '{1}'", n, _ops[op].name() );

//		std::cout << "re-using existing node " << n << std::endl;
		return n;
	}
	
	n = static_cast<node_id>( _nodes.size() );
//	std::cout << "graph " << this << ": new node " << n << " op " << _ops[op].name() << ": " << hv << std::endl;

	_nodes.emplace_back( node( op, d, inputs, std::move( value ), hv ) );
	_hash_to_node[hv] = n;

	for ( auto i: inputs )
		_nodes[i].add_output( n );

	return n;
}

////////////////////////////////////////

node_id
graph::add_node( op_id op, any value, const dimensions &d, const std::vector<node_id> &inputs, const hash::value &hv )
{
	node_id n = find_node( hv );
	if ( n != nullnode )
	{
		// simple collision check
		const node &colCheck = _nodes[n];
		if ( colCheck.op() != op || colCheck.input_size() != inputs.size() || colCheck.dims() != d )
			throw_logic( "Hash collision with existing node {0} ({1}, {2}) adding opname '{3}' dims {4}: {5} vs {6}", n, _ops[colCheck.op()].name(), colCheck.dims(), _ops[op].name(), d, colCheck.hash_value(), hv );
		
//		std::cout << "re-using existing node " << n << std::endl;
		return n;
	}
	
	n = static_cast<node_id>( _nodes.size() );
//	std::cout << "graph " << this << ": new node " << n << " op " << _ops[op].name() << ": " << hv << std::endl;
	_nodes.emplace_back( node( op, d, inputs, std::move( value ), hv ) );
	_hash_to_node[hv] = n;

	for ( auto i: inputs )
		_nodes[i].add_output( n );

	return n;
}

////////////////////////////////////////

node_id
graph::add_node( op_id op, any value, const dimensions &d, const std::vector<node_id> &inputs, hash &h )
{
	if ( _ops[op].input_size() != inputs.size() )
		throw_logic( "Invalid number of inputs passed to operator {0}, expect {1}, got {2}", _ops[op].name(), _ops[op].input_size(), inputs.size() );

	h << op << d;
	for ( auto n: inputs )
	{
		precondition( n < _nodes.size(), "Invalid input given for new node" );
		h << _nodes[n].hash_value();
	}
	hash::value hv = h.final();

	return add_node( op, std::move( value ), d, inputs, hv );
}

////////////////////////////////////////

void
graph::reference( node_id n, rewrite_notify notify, void *ud )
{
	precondition( n < _nodes.size(), "Invalid node for reference" );

	auto ri = _ref_counts.find( n );
	if ( ri != _ref_counts.end() )
		ri->second.emplace_back( notify, ud );
	else
	{
		_ref_counts[n].emplace_back( notify, ud );
		_nodes[n].set_ref();
	}
}

////////////////////////////////////////

void
graph::unreference( node_id n, rewrite_notify notify, void *ud ) noexcept
{
	precondition( n < _nodes.size(), "Invalid node for reference" );

	auto ri = _ref_counts.find( n );
	if ( ri != _ref_counts.end() )
	{
		reference_list &l = ri->second;
		for ( auto i = l.begin(); i != l.end(); ++i )
		{
			if ( (*i).first == notify && (*i).second == ud )
			{
				l.erase( i );
				break;
			}
		}
		if ( l.empty() )
		{
			_ref_counts.erase( ri );
			_nodes[n].clear_ref();
		}
	}
}

////////////////////////////////////////

} // engine



