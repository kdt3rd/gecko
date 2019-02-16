//
// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once

#include <iterator>

////////////////////////////////////////

namespace scene
{

class node;

/// This provides a consistent view of a node during the iterator lifetime
/// NB: if you implement a recursive algorithm to traverse the tree, you
/// may end up locking out any other threads from manipulating the 
class const_node_iterator : public std::iterator<std::random_access_iterator_tag, size_t>
{
	typedef std::iterator_traits<I> _traits;
public:
	using difference_type = typename _traits::difference_type;
	/// unary operators

	node_reference operator*( void ) const noexcept;
	node_reference *operator->( void ) const noexcept;
	node_iterator &operator++( void );
	node_iterator operator++( int );
	node_iterator &operator--( void );
	node_iterator operator--( int );

	column_iterator &operator+=( difference_type i );
	column_iterator &operator-=( difference_type i );
	column_iterator operator+( difference_type i ) const;
	column_iterator operator-( difference_type i ) const;

	difference_type operator-( const const_node_iterator &i ) const;

	node_reference operator[]( difference_type i ) const;
};

class writable_node_reference
{
};

/// this is a read-only node reference, if you need to modify the node
/// convert it to a writable_node_reference
class node_reference
{
public:
	using const_iterator = const_node_iterator;

	constexpr node_reference( nullptr_t ) noexcept;
	constexpr node_reference( void ) noexcept;
	~node_reference( void );
	node_reference( const node_reference &n );
	node_reference &operator=( const node_reference &n ) noexcept;
	node_reference( node_reference &&n ) noexcept;
	node_reference &operator=( node_reference &&n ) noexcept;

	node_reference parent( void ) { return _parent; }

	size_t size( void ) const;
	node_reference operator[]( size_t i ) const;
	node_reference operator[]( const char *n ) const;

	const_iterator begin( void ) const;
	const_iterator end( void ) const;
	
	const node &operator*( void ) const noexcept { return *_node; }
	const node *operator->( void ) const noexcept { return _node; }

	explicit operator bool( void ) const { return _node != nullptr; }

protected:
	explicit node_reference( scene &s, const node_reference &parent, node *n ) noexcept;
	friend class scene;

	scene &_scene;
	node_reference _parent;
	node *_node = nullptr;
};

/// Converts the node_reference to a writable reference.
///
/// the node_reference passed in will be invalid after this
writable_node_reference make_writable( node_reference &nr );
/// Converts the writable_node_reference back to a readable reference.
///
/// the node_reference passed in will be invalid after this
node_reference make_readable( writable_node_reference &nr );


// TODO: do we need comparisons against null?
bool operator==( const node_reference &a, const node_reference &b ) noexcept;
bool operator!=( const node_reference &a, const node_reference &b ) noexcept;
bool operator<( const node_reference &a, const node_reference &b ) noexcept;
bool operator<=( const node_reference &a, const node_reference &b ) noexcept;
bool operator>( const node_reference &a, const node_reference &b ) noexcept;
bool operator>=( const node_reference &a, const node_reference &b ) noexcept;
void swap( node_reference &a, node_reference &b );

/// TBD: do we want something like this???
template <typename T> inline const T *static_pointer_cast( const node_reference &r ) noexcept;
template <typename T> inline const T *dynamic_pointer_cast( const node_reference &r ) noexcept;
template <typename T> inline const T *reinterpret_pointer_cast( const node_reference &r ) noexcept;

} // namespace scene



