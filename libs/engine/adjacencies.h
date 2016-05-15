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
#include <initializer_list>

////////////////////////////////////////

namespace engine
{

/// @brief Store adjacent edges in the graph
///
/// it is assumed a higher level is avoiding cycles (or not)
/// Further, it is also required that the number of input edges be known at construction time
class adjacencies
{
public:
	adjacencies( void );
	~adjacencies( void );

	adjacencies( std::initializer_list<node_id> inputs );

	adjacencies( adjacencies &&a ) noexcept;
	adjacencies &operator=( adjacencies &&a ) noexcept;
	adjacencies( const adjacencies & );
	adjacencies &operator=( const adjacencies & );

	inline size_t input_size( void ) const;
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

	void clear( void );

private:
	void resize( size_t num_in, size_t num_out = 0 );

	node_id *_input_edges = nullptr;
	node_id *_output_edges = nullptr;
	uint32_t _storage_count = 0;
	uint32_t _output_count = 0;
};

////////////////////////////////////////

inline size_t adjacencies::input_size( void ) const
{
	return _output_edges - _input_edges;
}

////////////////////////////////////////

inline size_t adjacencies::output_size( void ) const
{
	return _output_count;
}

////////////////////////////////////////

inline const node_id *adjacencies::begin_inputs( void ) const
{
	return _input_edges;
}

////////////////////////////////////////

inline const node_id *adjacencies::end_inputs( void ) const
{
	return _output_edges;
}

////////////////////////////////////////

inline const node_id *adjacencies::begin_outputs( void ) const
{
	return _output_edges;
}

////////////////////////////////////////

inline const node_id *adjacencies::end_outputs( void ) const
{
	return _output_edges + _output_count;
}

inline node_id &adjacencies::input( size_t i )
{
	return _input_edges[i];
}

////////////////////////////////////////

inline node_id adjacencies::input( size_t i ) const
{
	return _input_edges[i];
}

////////////////////////////////////////

inline node_id &adjacencies::output( size_t i )
{
	return _output_edges[i];
}

////////////////////////////////////////

inline node_id adjacencies::output( size_t i ) const
{
	return _output_edges[i];
}

} // namespace engine



