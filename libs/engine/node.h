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

#include <vector>

#include "types.h"

////////////////////////////////////////

namespace engine
{

/// @brief Class node provides an entry for the graph to store
/// attributes about the processing represented.
///
/// The idea of the processing engine should be to support very large
/// graphs. As such, the memory layout of the node, and it's input
/// references should be kept to a minimum, and as tightly packed as
/// is possible.
///
class node
{
public:
	node( void );
	node( op_id o, const dimensions &d, std::initializer_list<node_id> inputs, any val, hash::value hv );
	node( op_id o, const dimensions &d, const std::vector<node_id> &inputs, any val, hash::value hv );

	node( const node &n );
	node &operator=( const node &n );
	node( node &&n ) noexcept;
	node &operator=( node &&n ) noexcept;

	~node( void );

	/// operation assigned to this node
	inline op_id op( void ) const;
	/// relative output dimensions for this node
	inline const dimensions &dims( void ) const;

	inline bool is_rvalue( void ) const;
	inline void set_rvalue( void );

	/// number of input nodes (nodes who have this node as an output,
	/// and the ordering of such)
	inline size_t input_size( void ) const;
	/// number of output nodes (nodes who have this node as an input)
	inline size_t output_size( void ) const;

	inline const node_id *begin_inputs( void ) const;
	inline const node_id *end_inputs( void ) const;

	inline const node_id *begin_outputs( void ) const;
	inline const node_id *end_outputs( void ) const;
	inline node_id &input( size_t i );
	inline node_id input( size_t i ) const;
	inline node_id &output( size_t i );
	inline node_id output( size_t i ) const;

	void add_output( node_id o );
	void remove_output( node_id o );

	inline const hash::value &hash_value( void ) const;
	inline any &value( void );
	inline const any &value( void ) const;


private:
	void resize_edges( uint32_t num_in, uint32_t num_out );

	// hmmm, is this any different than a node with only one output?
	// although with a dag of all rvalues, it's obvious that it can
	// trivially collapse into another dag and not share
	static constexpr int flag_rvalue = 0;

	// trying to be as compact as possible for storing hundreds of
	// thousands / millions of nodes, so instead
	// of using std::vector, we manage memory ourselves
	hash::value _hash; // 16 bytes
	dimensions _dims = nulldim; // 16 bytes
	any _value; // 16 bytes using c++14 impl, 8 using base::any currently
	node_id *_edges = nullptr; // 8 bytes
	uint32_t _input_count = 0; // 4
	uint32_t _output_count = 0; // 4

	uint32_t _storage_count = 0; // 4
	op_id _op_id = nullop; // 2 bytes
	// overkill for the one flag we have right now, but use uint16_t
	// for alignment
	uint16_t _flags = 0; // 2

	// should be packed on 8-byte boundary for 64-bytes
};

////////////////////////////////////////

inline op_id node::op( void ) const
{
	return _op_id;
}

////////////////////////////////////////

inline const dimensions &node::dims( void ) const
{
	return _dims;
}

////////////////////////////////////////

inline bool node::is_rvalue( void ) const
{
	return ( _flags & (1 << flag_rvalue) ) != 0;
}

////////////////////////////////////////

inline void node::set_rvalue( void )
{
	_flags |= (1 << flag_rvalue);
}


////////////////////////////////////////

inline size_t node::input_size( void ) const
{
	return _input_count;
}

////////////////////////////////////////

inline size_t node::output_size( void ) const
{
	return _output_count;
}

////////////////////////////////////////

inline const node_id *node::begin_inputs( void ) const
{
	return _edges;
}

////////////////////////////////////////

inline const node_id *node::end_inputs( void ) const
{
	return _edges + _input_count;
}

////////////////////////////////////////

inline const node_id *node::begin_outputs( void ) const
{
	return end_inputs();
}

////////////////////////////////////////

inline const node_id *node::end_outputs( void ) const
{
	return begin_outputs() + _output_count;
}

inline node_id &node::input( size_t i )
{
	return _edges[i];
}

////////////////////////////////////////

inline node_id node::input( size_t i ) const
{
	return _edges[i];
}

////////////////////////////////////////

inline node_id &node::output( size_t i )
{
	return _edges[_input_count + i];
}

////////////////////////////////////////

inline node_id node::output( size_t i ) const
{
	return _edges[_input_count + i];
}

////////////////////////////////////////

inline const hash::value &
node::hash_value( void ) const
{
	return _hash;
}

////////////////////////////////////////

inline any &
node::value( void )
{
	return _value;
}

////////////////////////////////////////

inline const any &
node::value( void ) const
{
	return _value;
}

} // namespace engine



