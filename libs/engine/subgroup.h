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
#include "types.h"
#include <vector>
#include <set>
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

	bool is_output( node_id n ) const;
	bool is_member( node_id n ) const;

	/// returns the last input node that needs
	/// computation
	node_id last_input( void ) const;

	/// returns the first member
	node_id first_member( void ) const;

	void swap( subgroup &o );

	void process( void );
	any &output_val( size_t i );
	subgroup_function &func( size_t i );
	subgroup_function &func( node_id n );

private:
	void bind_functions( void );

	graph &_graph;
	std::set<node_id> _inputs;
	std::vector<node_id> _nodes;
	std::vector<node_id> _outputs;

	std::vector<std::shared_ptr<subgroup_function>> _funcs;
	std::vector<size_t> _output_funcidx;
	bool _processed = false;
};

} // namespace engine



