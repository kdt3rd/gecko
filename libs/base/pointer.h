
#pragma once

#include <stdlib.h>
#include <memory>

namespace base
{

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

/// @brief Wrap a C pointer such that it gets deleted properly
template<typename T>
std::unique_ptr<T,free_deleter> wrap_cptr( T *ptr )
{
	return std::unique_ptr<T,free_deleter>( ptr );
}

////////////////////////////////////////

}
