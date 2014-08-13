
#pragma once

#include "contract.h"
#include <list>
#include <array>
#include <vector>

namespace base
{

////////////////////////////////////////

template<typename T, size_t blocks>
class memory_pool
{
public:
	typedef T value_type;

	typedef T *pointer;
	typedef const T *const_pointer;

	typedef T &reference;
	typedef const T &const_reference;

	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	template <typename U> struct rebind { typedef memory_pool<U,blocks> other; };

	pointer address( reference x ) const
	{
		return &x;
	}

	const_pointer address( const_reference x ) const
	{
		return &x;
	}

	pointer allocate( size_type n = 1, const_pointer hint = 0 )
	{
		precondition( n == 1, "memory pool can only allocate 1 item" );

		// Allocate blocks if needed
		if ( _freelist.empty() )
		{
			_blocks.emplace_back();
			pointer result = &( _blocks.back()[0] );
			for ( size_t i = 1; i < blocks; ++i )
				_freelist.push_back( &( _blocks.back()[i] ) );
			return result;
		}

		pointer result = _freelist.back();
		_freelist.pop_back();
		return result;
	}

	void deallocate( pointer p, size_type n = 1 )
	{
		precondition( n == 1, "memory pool can only deallocate 1 item" );
		_freelist.push_back( p );
	}

	size_type max_size( void ) const
	{
		return 1;
	}

	template<typename U, typename ...Args>
	void construct( U *p, Args &&...args )
	{
		::new((void *)p) U( std::forward<Args>( args )... );
	}

	template<typename U>
	void destroy( U *p )
	{
		p->~U();
	}

	size_t available( void ) const
	{
		return _freelist.size();
	}

	size_t capacity( void ) const
	{
		return _blocks.size() * blocks;
	}

private:
	std::list<std::array<T,blocks>> _blocks;
	std::vector<pointer> _freelist;
};

////////////////////////////////////////

}

