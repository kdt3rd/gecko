//
// Copyright (c) 2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

namespace base
{

////////////////////////////////////////

// Priority queue, but with the ability to erase an element.
template<typename Key>
class priority_queue
{
public:
	priority_queue( size_t size, const std::function<bool(Key,Key)> &comp = std::greater<Key>() )
		: _comp( comp )
	{
		if ( size > 0 )
			_heap.reserve( size );
	}

	priority_queue( const std::function<bool(Key,Key)> &comp = std::greater<Key>() )
		: _comp( comp )
	{
	}

	void set_default( Key k )
	{
		_default = k;
	}

	void init( void )
	{
		std::sort( _heap.begin(), _heap.end(), _comp );
		_initialized = true;
	}

	bool empty( void ) const
	{
		return _heap.empty();
	}

	size_t size( void ) const
	{
		return _heap.size();
	}

	void reserve( size_t s )
	{
		_heap.reserve( s );
	}

	void clear( void )
	{
		_heap.clear();
	}

	Key top( void ) const
	{
		if ( _heap.empty() )
			return _default;
		return _heap.back();
	}

	void push( Key k )
	{
		if ( _initialized )
			_heap.insert( std::upper_bound( _heap.begin(), _heap.end(), k, _comp ), k );
		else
			_heap.push_back( k );
	}

	void pop( void )
	{
		precondition( _initialized, "priority_queue not initialized" );
		precondition( !empty(), "priority_queue is empty" );
		_heap.pop_back();
	}

	void erase( Key k )
	{
		auto i = std::lower_bound( _heap.begin(), _heap.end(), k, _comp );
		precondition( i != _heap.end(), "key not found in priority_queue" );

		auto j = i + 1;
		if ( j != _heap.end() )
			std::rotate( i, j, _heap.end() );
		_heap.pop_back();
	}

	bool is_sorted( void ) const
	{
		return std::is_sorted( _heap.begin(), _heap.end(), _comp );
	}

	Key operator[]( size_t i ) const
	{
		return _heap.at( i );
	}

private:
	bool _initialized = false;
	std::vector<Key> _heap;
	std::function<bool(Key,Key)> _comp;
	Key _default;
};

////////////////////////////////////////

template<typename Key>
std::ostream &operator<<( std::ostream &out, const priority_queue<Key> &q )
{
	for ( size_t i = 0; i < q.size(); ++i )
	{
		if ( i > 0 )
			out << ',';
		out << q[i];
	}
	return out;
}

////////////////////////////////////////

}

