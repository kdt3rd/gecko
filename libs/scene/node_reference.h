//
// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once

////////////////////////////////////////

namespace scene
{

class node;

class node_reference
{
public:
	constexpr node_reference( nullptr_t ) noexcept;
	constexpr node_reference( void ) noexcept;
	explicit node_reference( node *n ) noexcept;
	~node_reference( void );
	node_reference( const node_reference &n );
	node_reference &operator=( const node_reference &n ) noexcept;
	node_reference( node_reference &&n ) noexcept;
	node_reference &operator=( node_reference &&n ) noexcept;

	node *get( void ) const noexcept;

	node &operator*( void ) const noexcept;
	node *operator->( void ) const noexcept;

	explicit operator bool( void ) const;
};

// TODO: do we need comparisons against null?
bool operator==( const node_reference &a, const node_reference &b ) noexcept;
bool operator!=( const node_reference &a, const node_reference &b ) noexcept;
bool operator<( const node_reference &a, const node_reference &b ) noexcept;
bool operator<=( const node_reference &a, const node_reference &b ) noexcept;
bool operator>( const node_reference &a, const node_reference &b ) noexcept;
bool operator>=( const node_reference &a, const node_reference &b ) noexcept;
void swap( node_reference &a, node_reference &b );
template <typename T> inline T *static_pointer_cast( const node_reference &r ) noexcept;
template <typename T> inline T *dynamic_pointer_cast( const node_reference &r ) noexcept;
template <typename T> inline T *reinterpret_pointer_cast( const node_reference &r ) noexcept;

} // namespace scene



