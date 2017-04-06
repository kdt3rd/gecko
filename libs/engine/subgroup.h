//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once
#include "types.h"
#include <vector>
#include <set>
#include <mutex>
#include "subgroup_function.h"

////////////////////////////////////////

namespace engine
{

class graph;

/// @brief Class subgroup a mechanism for graphs to record subgroups
///
/// These will be used primarily for grouping scanline-based
/// operations together to be processed at once
class subgroup
{
public:
	explicit subgroup( graph &g );
	subgroup( graph &g, node_id first );
	subgroup( const subgroup & ) = default;
	subgroup( subgroup && ) = default;
	subgroup &operator=( const subgroup & ) = default;
	subgroup &operator=( subgroup && ) = default;
	~subgroup( void );

	inline graph &gref( void ) { return _graph; }
	inline const graph &gref( void ) const { return _graph; }

	inline bool empty( void ) const { return _nodes.empty(); }
	inline size_t size( void ) const { return _nodes.size(); }

	void clear( void );

	inline bool processed( void ) const { return _processed; }

	void add( node_id n );
	inline const std::vector<node_id> &members( void ) const { return _nodes; }
	inline const std::vector<node_id> &outputs( void ) const { return _outputs; }
	inline const std::set<node_id> &inputs( void ) const { return _inputs; }

	bool is_output( node_id n ) const;
	bool is_member( node_id n ) const;

	bool can_merge( const subgroup &o ) const;

	/// returns the last input node that needs
	/// computation
	node_id last_input( void ) const;

	/// returns the first member
	node_id first_member( void ) const;

	void swap( subgroup &o );

	void process( void );
	any &output_val( size_t i );
	size_t func_idx( node_id n );
	void bind_functions( std::vector<std::shared_ptr<subgroup_function>> &funcs );

private:
	bool internal_merge_check( const subgroup &o ) const;

	graph &_graph;
	std::set<node_id> _inputs;
	std::vector<node_id> _nodes;
	std::vector<node_id> _outputs;

	bool _processed = false;
};

} // namespace engine



