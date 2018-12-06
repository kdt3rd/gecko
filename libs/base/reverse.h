//
// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT
//

#pragma once

#include <utility>
#include <iterator>

namespace base
{

////////////////////////////////////////

/// @brief Wrap a container to reverse it's order
template<class Cont>
class const_reverse_wrapper
{
private:
	const Cont &_container;

public:
	const_reverse_wrapper( const Cont &cont )
		: _container( cont )
	{
	}

	decltype(_container.rbegin()) begin( void ) const
	{
		return _container.rbegin();
	}

	decltype(_container.rend()) end( void ) const
	{
		return _container.rend();
	}
};

////////////////////////////////////////

/// @brief Wrap a container to reverse it's order
template<class Cont>
class reverse_wrapper
{
private:
	Cont &_container;

public:
	reverse_wrapper( Cont &cont )
		: _container( cont )
	{
	}

	decltype(_container.rbegin()) begin( void )
	{
		return _container.rbegin();
	}

	decltype(_container.rend()) end( void )
	{
		return _container.rend();
	}
};

////////////////////////////////////////

/// @brief Wrap the container to reverse it's order
template<class Cont>
const_reverse_wrapper<Cont> reverse( const Cont &cont )
{
	return const_reverse_wrapper<Cont>( cont );
}

////////////////////////////////////////

/// @brief Wrap the container to reverse it's order
template<class Cont>
reverse_wrapper<Cont> reverse( Cont &cont )
{
	return reverse_wrapper<Cont>( cont );
}

////////////////////////////////////////

}

