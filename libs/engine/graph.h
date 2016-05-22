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

#pragma once

#include <typeinfo>
#include <vector>
#include <ostream>
#include <base/const_string.h>

#include "types.h"
#include "node.h"
#include "registry.h"

////////////////////////////////////////

namespace engine
{

///
/// @brief Class graph provides storage for a graph of operations
///
/// The idea is to have this graph be dynamic in nature, and storing
/// what is to be done until a value is needed, at which point the
/// graph as it exists can be optimized based on what is known, and a
/// request processed, which may only be a sub-portion of the
/// graph. Then the graph may be continued to be edited, with the
/// previously processed (and no longer referenced) portions
/// discarded at some point.
///
/// How this happens in the end remains to be seen. For now, the above
/// behavior described is very explicit.
///
class graph
{
public:
	typedef std::vector<node>::iterator iterator;
	typedef std::vector<node>::const_iterator const_iterator;
	typedef std::vector<node>::reverse_iterator reverse_iterator;
	typedef std::vector<node>::const_reverse_iterator const_reverse_iterator;

	/// The registry must exist for the lifetime of the graph
	explicit graph( const registry &o );
	~graph( void );

	inline const registry &op_registry( void ) const;

	template <typename V>
	inline node_id add_constant( V &&v )
	{
		hash h;
		h << v;
		return add_node( _ops.find_constant( typeid(V) ), any( std::move( v ) ), nulldim, {}, h );
	}

	template <typename V>
	inline node_id add_constant( const V &v )
	{
		hash h;
		h << v;
		return add_node( _ops.find_constant( typeid(V) ), any( v ), nulldim, {}, h );
	}

	node_id add_node( const base::cstring &opname, const dimensions &d, std::initializer_list<node_id> inputs );

	void tag_rvalue( node_id n );

	const any &get_value( node_id n );

	node_id copy_node( const graph &o, node_id n );
	node_id move_node( graph &o, node_id n );
	void remove_node( node_id n );

	inline size_t size( void ) const { return _nodes.size(); }
	inline const node &operator[]( node_id n ) const { return _nodes[n]; }

	inline iterator begin( void ) { return _nodes.begin(); }
	inline const_iterator begin( void ) const { return _nodes.begin(); }
	inline const_iterator cbegin( void ) const { return _nodes.cbegin(); }
	inline iterator end( void ) { return _nodes.end(); }
	inline const_iterator end( void ) const { return _nodes.end(); }
	inline const_iterator cend( void ) const { return _nodes.cend(); }

	inline reverse_iterator rbegin( void ) { return _nodes.rbegin(); }
	inline const_reverse_iterator rbegin( void ) const { return _nodes.rbegin(); }
	inline const_reverse_iterator crbegin( void ) const { return _nodes.crbegin(); }
	inline reverse_iterator rend( void ) { return _nodes.rend(); }
	inline const_reverse_iterator rend( void ) const { return _nodes.rend(); }
	inline const_reverse_iterator crend( void ) const { return _nodes.crend(); }

	// remove
	void clean_graph( void );

	void dispatch_threads( const std::function<void(int, int)> &f, int start, int N );

	void dump_dot( std::ostream &os );
	void dump_refs( std::ostream &os );

private:
	graph( void ) = delete;
	graph( const graph & ) = delete;
	graph( graph && ) = delete;
	graph &operator=( const graph & ) = delete;
	graph &operator=( graph && ) = delete;

	const any &start_process( node_id nid, node &n );
	const any &process( node_id nid, node &n );
	void optimize( void );
	void apply_grouping( void );

	node_id find_node( const hash::value &hv );

	node_id add_node( op_id op, any value, const dimensions &d, std::initializer_list<node_id> inputs );
	node_id add_node( op_id op, any value, const dimensions &d, std::initializer_list<node_id> inputs, hash &h );
	node_id add_node( op_id op, any value, const dimensions &d, std::initializer_list<node_id> inputs, const hash::value &hv );
	node_id add_node( op_id op, any value, const dimensions &d, const std::vector<node_id> &inputs, const hash::value &hv );
	node_id add_node( op_id op, any value, const dimensions &d, const std::vector<node_id> &inputs, hash &h );


//	template <typename V> friend class computed_value;
	friend class computed_base;

	typedef void (*rewrite_notify)( void *, node_id, node_id );

	void reference( node_id n, rewrite_notify notify, void *ud );
	void unreference( node_id n, rewrite_notify notify, void *ud ) noexcept;

	const registry &_ops;

	std::vector<node> _nodes;
	typedef std::vector< std::pair<rewrite_notify,void *> > reference_list;
	std::map<node_id, reference_list> _ref_counts;
	std::map<hash::value, node_id> _hash_to_node;
};

////////////////////////////////////////

inline const registry &graph::op_registry( void ) const
{
	return _ops;
}


} // namespace engine



