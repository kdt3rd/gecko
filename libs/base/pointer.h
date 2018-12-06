//
// Copyright (c) 2014-2016 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once

#include <stdlib.h>
#include <memory>

namespace base
{

////////////////////////////////////////

/// @brief do not free pointed-to-memory (BEWARE)
struct no_deleter
{
	void operator()( void * )
	{
	}
};

////////////////////////////////////////

/// @brief Delete a pointer using free
struct free_deleter
{
	void operator()( void *ptr )
	{
		::free( ptr );
	}
};

////////////////////////////////////////

/// @brief Delete a pointer using free
template<typename T>
struct array_deleter
{
	void operator()( T *ptr )
	{
		delete[] ptr;
	}
};

////////////////////////////////////////

/// @brief Wrap a C pointer such that it gets deleted properly
template<typename T>
std::unique_ptr<T,free_deleter> wrap_cptr( T *ptr )
{
	return std::unique_ptr<T,free_deleter>( ptr );
}

////////////////////////////////////////

}
