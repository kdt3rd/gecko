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
#include <base/contract.h>
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

	/// flag indicating that the node is an r-value
	/// meaning no references and only one output
	inline bool is_rvalue( void ) const;
	/// set the reference flag
	inline void set_rvalue( void );
	/// clear the reference flag
	inline void clear_rvalue( void );
	/// flag indicating that the node has at least one reference to it

	inline bool has_ref( void ) const;
	/// set the reference flag
	inline void set_ref( void );
	/// clear the reference flag
	inline void clear_ref( void );

	/// test whether a user flag is set
	/// (flag the node doesn't know about)
	/// valid values are 0 - 7
	inline bool is_user_flag_set( int f );
	/// set a flag the node doesn't know about
	/// valid values are 0 - 7
	inline void set_user_flag( int f );
	/// clear a flag the node doesn't know about
	/// valid values are 0 - 7
	inline void clear_user_flag( int f );


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
	static constexpr int flag_rvalue = 0;
	static constexpr int flag_hasref = 1;

	inline bool is_set( int f ) const
	{
		return ( _flags & (1 << f) ) != 0;
	}
	inline void set_flag( int f )
	{
		_flags |= (1 << f);
	}
	inline void clear_flag( int f )
	{
		_flags &= ~(1 << f);
	}
	void resize_edges( uint32_t num_in, uint32_t num_out );

	// hmmm, is this any different than a node with only one output?
	// although with a dag of all rvalues, it's obvious that it can
	// trivially collapse into another dag and not share

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
	return is_set( flag_rvalue );
}

////////////////////////////////////////

inline void node::set_rvalue( void )
{
	set_flag( flag_rvalue );
}

////////////////////////////////////////

inline void node::clear_rvalue( void )
{
	clear_flag( flag_rvalue );
}

////////////////////////////////////////

inline bool node::has_ref( void ) const
{
	return is_set( flag_hasref );
}

////////////////////////////////////////

inline void node::set_ref( void )
{
	set_flag( flag_hasref );
}

////////////////////////////////////////

inline void node::clear_ref( void )
{
	clear_flag( flag_hasref );
}

////////////////////////////////////////

inline bool node::is_user_flag_set( int f )
{
	precondition( f >= 0 && f < 7, "invalid flag" );
	return is_set( f + 8 );
}

////////////////////////////////////////

inline void node::set_user_flag( int f )
{
	precondition( f >= 0 && f < 7, "invalid flag" );
	set_flag( f + 8 );
}

////////////////////////////////////////

inline void node::clear_user_flag( int f )
{
	precondition( f >= 0 && f < 7, "invalid flag" );
	clear_flag( f + 8 );
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



