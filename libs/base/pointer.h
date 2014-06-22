
#pragma once

#include <stdlib.h>
#include <memory>

namespace base
{

////////////////////////////////////////

struct free_deleter
{
	void operator()( void *ptr )
	{
		::free( ptr );
	}
};

////////////////////////////////////////

template<typename T>
std::unique_ptr<T,free_deleter> wrap_cptr( T *ptr )
{
	return std::unique_ptr<T,free_deleter>( ptr );
}

////////////////////////////////////////

}
