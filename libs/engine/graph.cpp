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
#include <base/contract.h>
#include <base/compiler_abi.h>
#include <iomanip>

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
		return process( n, node );

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
			else if ( curIn.output_size() == 1 )
			{
				auto rc = _ref_counts.find( *b );
				// no more references, let's schedule it for deletion
				if ( rc == _ref_counts.end() )
					del.push_back( *b );
			}
			std::cout << "remove node " << n << " from outputs of " << *b << std::endl;
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
			// dangling output
			auto ref = _ref_counts.find( nid );
			if ( n.output_size() == 0 && ref == _ref_counts.end()  )
			{
				toDel.insert( nid );
				didAdd = true;
				continue;
			}

			if ( ref != _ref_counts.end() )
				continue;

			size_t validOuts = 0;
			for ( size_t o = 0, nO = n.output_size(); o != nO; ++o )
			{
				if ( toDel.find( n.output( o ) ) == toDel.end() )
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
				std::cout << "removing output node " << oldout << std::endl;
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

void
graph::dispatch_threads( const std::function<void(int, int)> &f, int start, int N )
{
	f( start, N );
}

////////////////////////////////////////

void
graph::dump_dot( std::ostream &os )
{
	os << "digraph graph_0x" << this <<
		" {\n"
	    "graph [label=\"green means has value\\nblue means has reference\\nred means dangling no output\"]\n"
	    "node [shape=record]\n"
		"\n";

	for ( size_t n = 0, N = _nodes.size(); n != N; ++n )
	{
		const node &curN = _nodes[n];
		if ( curN.op() == nullop )
			continue;

		os << 'N' << n << " [label=\"";
		if ( curN.input_size() > 0 )
		{
			os << "{{";
			for ( size_t i = 0, nI = curN.input_size(); i != nI; ++i )
			{
				if ( i > 0 )
					os << '|';
				if ( curN.input( i ) == nullnode )
					os << '<' << i << "> NIL ";
				else
					os << '<' << i << "> " << base::demangle( _ops[curN.op()].input_type( i ) );
			}
			os << "}| N" << n << "\\n" << _ops[curN.op()].name() << "\\n" << curN.hash_value()
			   << '}';
		}
		else
			os << 'N' << n << "\\n" << _ops[curN.op()].name() << "\\n" << curN.hash_value();
		os << '\"';
		if ( _ref_counts.find( static_cast<node_id>( n ) ) != _ref_counts.end() )
			os << ", style=filled, fillcolor=blue";
		else if ( curN.output_size() == 0 )
			os << ", style=filled, fillcolor=red";
		else if ( ! curN.value().empty() )
			os << ", style=filled, fillcolor=green";

		os << "]\n";
	}

	// done all the nodes, now do the edges
	for ( size_t n = 0, N = _nodes.size(); n != N; ++n )
	{
		const node &curN = _nodes[n];
		if ( curN.op() == nullop )
			continue;

		for ( size_t i = 0, nI = curN.input_size(); i != nI; ++i )
		{
			if ( curN.input( i ) == nullnode )
				continue;

			os << 'N' << curN.input( i ) << " -> N" << n << ":" << i << '\n';
		}
	}
	
	os << "\n}\n";
}

////////////////////////////////////////

void
graph::dump_refs( std::ostream &os )
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

const any &
graph::process( node_id nid, node &n )
{
	std::cout << "process: " << n.hash_value() << " " << _ops[n.op()].name() << std::endl;
	optimize();
	apply_grouping();

	// by construction, if a node is at node_id nid, all it's inputs
	// appear before
	// can we just process up to that node and not bother trying to
	// compute which ones to process or not to process? may as well

	std::vector<any> inputs;

	for ( node_id c = 0; c <= nid; ++c )
	{
		node &curN = _nodes[c];
		if ( curN.op() == nullop )
			continue;
		if ( ! curN.value().empty() )
			continue;

		std::cout << "  computing node " << c << std::endl;
		size_t nInputs = curN.input_size();
		inputs.resize( nInputs );
		for ( size_t i = 0; i != nInputs; ++i )
		{
			node_id inid = curN.input( i );
			node &curIn = _nodes[inid];
			inputs[i] = curIn.value();
		}
		std::cout << "    -> inputs assigned, calling function" << std::endl;

		const op &o = _ops[curN.op()];
		curN.value() = o.function().process( *this, curN.dims(), inputs );

		std::cout << "    -> updating outputs..." << std::endl;
		for ( size_t i = 0; i != nInputs; ++i )
		{
			node_id &inid = curN.input( i );
			node &curIn = _nodes[inid];
			if ( curIn.is_rvalue() )
			{
				std::cout << "removing r-value input node " << inid << " now that we've processed it" << std::endl;
				remove_node( inid );
			}
			else
			{
				std::cout << "removing us (" << c << ") from " << inid << " now that we've processed it" << std::endl;
				curIn.remove_output( c );
				if ( curIn.output_size() == 0 )
				{
					std::cout << "removing input node " << inid << " now that it has no more unprocessed outputs" << std::endl;
					remove_node( inid );
				}
			}
			// notify any future remove_node that
			// we've already removed us from our input
			inid = nullnode;
		}
	}

	return n.value();
}

////////////////////////////////////////

void
graph::optimize( void )
{
	for ( size_t n = 0, N = _nodes.size(); n != N; ++n )
	{
		node &cur = _nodes[n];
		// only 1 output and no remaining references in
		// computed_value, we can tag as an rvalue
		if ( cur.output_size() == 1 &&
			 _ref_counts.find( static_cast<node_id>( n ) ) == _ref_counts.end() )
			cur.set_rvalue();
	}
}

////////////////////////////////////////

void
graph::apply_grouping( void )
{
}

////////////////////////////////////////

node_id
graph::find_node( const hash::value &hv )
{
	std::cout << "searching for hv " << hv << std::endl;
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

		std::cout << "re-using existing node " << n << std::endl;
		return n;
	}
	
	n = static_cast<node_id>( _nodes.size() );
	std::cout << "graph " << this << ": new node " << n << " op " << _ops[op].name() << ": " << hv << std::endl;

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
		
		std::cout << "re-using existing node " << n << std::endl;
		return n;
	}
	
	n = static_cast<node_id>( _nodes.size() );
	std::cout << "graph " << this << ": new node " << n << " op " << _ops[op].name() << ": " << hv << std::endl;
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
	auto ri = _ref_counts.find( n );
	if ( ri != _ref_counts.end() )
		ri->second.emplace_back( notify, ud );
	else
		_ref_counts[n].emplace_back( notify, ud );
}

////////////////////////////////////////

void
graph::unreference( node_id n, rewrite_notify notify, void *ud ) noexcept
{
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
			_ref_counts.erase( ri );
	}
}

////////////////////////////////////////

} // engine



