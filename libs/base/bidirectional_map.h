//
// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT
// Copyrights licenced under the MIT License.
//

#pragma once

#include <vector>
#include <utility>
#include <stdexcept>
#include "contract.h"

////////////////////////////////////////

namespace base
{

///
/// @brief Class bidirectional_map provides a simple two-way lookup scheme.
///
/// NB: This is intended for small sets of data, where a sorted list
/// and binary search can be used for the primary (A) key, but a
/// linear scan of B is feasible. if this becomes a performance issue,
/// the implementation can morph to a more boost-style implementation,
/// but that seems more difficult and error prone
///
template <typename A, typename B, class Allocator = std::allocator< std::pair<A, B> > >
class bidirectional_map
{
public:
	using value_type_A = A;
	using reference_A = A &;
	using const_reference_A = const A &;
	using value_type_B = B;
	using reference_B = B &;
	using const_reference_B = const B &;
	using value_type = std::pair<A, B>;
	using reference = value_type &;
	using const_reference = const value_type &;
	using allocator_type = Allocator;
	using container_type = std::vector<value_type, Allocator>;
	using size_type = typename container_type::size_type;
	using iterator = typename container_type::iterator;
	using const_iterator = typename container_type::const_iterator;
	using reverse_iterator = typename container_type::reverse_iterator;
	using const_reverse_iterator = typename container_type::const_reverse_iterator;

	bidirectional_map( void ) = default;
	bidirectional_map( const bidirectional_map & ) = default;
	bidirectional_map( bidirectional_map && ) = default;
	~bidirectional_map( void ) = default;
	bidirectional_map &operator=( const bidirectional_map & ) = default;
	bidirectional_map &operator=( bidirectional_map && ) = default;

	// don't know when to sort, so can't provide this currently
//	inline reference_A operator[]( const value_type_B &b )
//	{
//		iterator i = find( b );
//		if ( i == end() )
//		{
//			auto r = emplace( value_type_A(), b );
//			postcondition( r.second, "expect to insert item" );
//			return r.first->first;
//		}
//		return i->first;
//	}

	inline const_reference_A operator[]( const value_type_B &b ) const
	{
		const_iterator i = find( b );
		if ( i == end() )
			throw std::out_of_range("bidirectional_map operator[] item not found");
		return i->first;
	}
	inline reference_B operator[]( const value_type_A &a )
	{
		iterator i = find( a );
		if ( i == end() )
		{
			_m.emplace_back( a, value_type_B() );
			std::stable_sort( std::begin( _m ), std::end( _m ) );
			i = find( a );
			postcondition( i != end(), "expect to insert and find item" );
		}
		return i->second;
	}
	inline const_reference_B operator[]( const value_type_A &a ) const
	{
		const_iterator i = find( a );
		if ( i == end() )
			throw std::out_of_range("bidirectional_map operator[] item not found");
		return i->second;
	}

	inline iterator begin( void ) { return _m.begin(); }
	inline iterator end( void ) { return _m.end(); }
	inline const_iterator begin( void ) const { return _m.begin(); }
	inline const_iterator end( void ) const { return _m.end(); }
	inline const_iterator cbegin( void ) const { return _m.cbegin(); }
	inline const_iterator cend( void ) const { return _m.cend(); }

	inline reverse_iterator rbegin( void ) { return _m.rbegin(); }
	inline reverse_iterator rend( void ) { return _m.rend(); }
	inline const_iterator rbegin( void ) const { return _m.rbegin(); }
	inline const_iterator rend( void ) const { return _m.rend(); }
	inline const_iterator crbegin( void ) const { return _m.crbegin(); }
	inline const_iterator crend( void ) const { return _m.crend(); }

	inline bool empty( void ) const { return _m.empty(); }
	inline size_type size( void ) const { return _m.size(); }
	inline size_type max_size( void ) const { return _m.max_size(); }
	inline void reserve( size_type N ) { _m.reserve( N ); }
	inline size_t capacity( void ) const { return _m.capacity(); }
	inline void shrink_to_fit( void ) { _m.shrink_to_fit(); }

	inline void clear( void ) { _m.clear(); }

	inline void swap( bidirectional_map &o ) { _m.swap( o._m ); }

	// TODO: do we need to provide front/back/data/lower_bound/etc?

	inline size_type count( const value_type_A &a ) const
	{
		return ( find( a ) == end() ) ? 0 : 1;
	}
	inline size_type count( const value_type_B &b ) const
	{
		return ( find( b ) == end() ) ? 0 : 1;
	}

	/// NB: we don't provide the templated find in c++14 because which
	/// do we use as the key??? maybe we could do something magical w/
	/// is_convertible?
	inline iterator find( const value_type_A &a )
	{
		// we are sorted by a, so can do log(N) search
		return std::lower_bound( begin(), end(), a, []( value_type &x, const value_type_A &a ) { return x.first == a; } );
	}
	inline const_iterator find( const value_type_A &a ) const
	{
		// we are sorted by a, so can do log(N) search
		return std::lower_bound( begin(), end(), a, []( const value_type &x, const value_type_A &a ) { return x.first == a; } );
	}
	inline iterator find( const value_type_B &b )
	{
		// we aren't sorted by b so can't do log(N) search
		return std::find_if( begin(), end(), [&]( value_type &x ) { return x.second == b; } );
	}
	inline const_iterator find( const value_type_B &b ) const
	{
		// we aren't sorted by b so can't do log(N) search
		return std::find_if( begin(), end(), [&]( const value_type &x ) { return x.second == b; } );
	}

	template <typename... Args>
	inline std::pair<iterator, bool> emplace( Args &&... args )
	{
		value_type v{ std::forward<Args>( args )... };
		iterator i = std::lower_bound( begin(), end(), v );
		if ( i != end() && !(v < *i) )
			return std::make_pair( i, false );

		_m.emplace_back( std::move( v ) );
		std::stable_sort( std::begin( _m ), std::end( _m ) );
		i = std::lower_bound( begin(), end(), v );
		postcondition( i != end() && !(v < *i), "expect to insert and find item" );
		return std::make_pair( i, true );
	}

	inline void erase( iterator pos )
	{
		_m.erase( pos );
	}

	inline iterator erase( const_iterator pos )
	{
		return _m.erase( pos );
	}

	inline size_type erase( const value_type_A &a )
	{
		size_type count = 0;
		iterator i = find( a );
		if ( i != end() )
		{
			erase( i );
			++count;
		}
		return count;
	}

	inline size_type erase( const value_type_B &b )
	{
		size_type cnt = 0;
		iterator i = find( b );
		if ( i != end() )
		{
			erase( i );
			++cnt;
		}
		return cnt;
	}

private:
	container_type _m;
};

} // namespace base

namespace std
{

template <class A, class B, class Allocator>
inline void swap( base::bidirectional_map<A, B, Allocator> &a,
				  base::bidirectional_map<A, B, Allocator> &b )
{
	a.swap( b );
}

}


