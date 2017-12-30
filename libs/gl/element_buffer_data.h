//
// Copyright (c) 2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "vector.h"
#include "color.h"
#include <base/meta.h>
#include <base/tuple_util.h>
#include <utility>

namespace gl
{

////////////////////////////////////////

/// @brief Buffer data is an array of indices.
class element_buffer_data
{
public:
	/// @brief Default constructor.
	element_buffer_data( void )
	{
	}

	/// @brief Initializer list constructor.
	/// @param list List of tuples (one entry per vertex).
	template<typename I>
	element_buffer_data( std::initializer_list<I> list )
		: _data( list.begin(), list.end() )
	{
	}

	/// @brief Add an index.
	/// @param i Index to add.
	/// @param args More indices to add.
	template<typename ...Args>
	void push_back( uint32_t i, Args &&...args )
	{
		precondition( !_ebo, "cannot add index after creating buffer object" );
		_data.push_back( static_cast<uint32_t>( i ) );
		push_back( std::forward<Args>( args )... );
	}

	/// @brief Remove last index added.
	void pop_back( void )
	{
		precondition( !_ebo, "cannot remove index after creating buffer object" );
		_data.pop_back();
	}

	/// @brief Returns the number of indices.
	size_t size( void ) const
	{
		return _data.size();
	}

	/// @brief Returns true if the buffer is empty.
	bool empty( void ) const
	{
		return _data.empty();
	}

	/// @brief Clear the buffer of all data.
	void clear( void )
	{
		_data.clear();
	}

	/// @brief The raw uint32_t data of all indices.
	/// There will be size() uint32_t in the array.
	/// @returns Pointer to raw data.
	const uint32_t *data( void ) const
	{
		return _data.data();
	}

	/// @brief Return the ith index.
	/// @param i Index of index to return.
	/// @returns ith index in array.
	uint32_t operator[]( size_t i ) const
	{
		return _data[i];
	}

	/// @brief Element buffer object for the data.
	/// The EBO is created as needed.
	/// Once a EBO is created, the data cannot be modified.
	/// @returns The element buffer object.
	std::shared_ptr<element_buffer> ebo( buffer_usage u = buffer_usage::STATIC_DRAW )
	{
		if ( !_ebo )
			_ebo = std::make_shared<element_buffer>( _data, u );
		return _ebo;
	}

private:
	void push_back( void )
	{
	}

	std::shared_ptr<element_buffer> _ebo;
	std::vector<uint32_t> _data;
};

////////////////////////////////////////

}
