//
// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT
//

#include <base/memory_pool.h>
#include <fstream>

namespace
{

struct test
{
	int x;
	float y;
};

int safemain( void )
{
	base::memory_pool<test,10> pool;
	std::cout << "Pool capacity: " << pool.available() << '/' << pool.capacity() << std::endl;

	for ( size_t i = 0; i < 5; ++i )
		pool.deallocate( pool.allocate() );
	std::cout << "Pool capacity: " << pool.available() << '/' << pool.capacity() << std::endl;

	std::vector<test *> pointers;

	for ( size_t i = 0; i < 5; ++i )
		pointers.push_back( pool.allocate() );
	std::cout << "Pool capacity: " << pool.available() << '/' << pool.capacity() << std::endl;

	for ( size_t i = 0; i < 10; ++i )
		pointers.push_back( pool.allocate() );
	std::cout << "Pool capacity: " << pool.available() << '/' << pool.capacity() << std::endl;

	for ( size_t i = 0; i < 5; ++i )
	{
		pool.deallocate( pointers.back() );
		pointers.pop_back();
	}
	std::cout << "Pool capacity: " << pool.available() << '/' << pool.capacity() << std::endl;

	while ( !pointers.empty() )
	{
		pool.deallocate( pointers.back() );
		pointers.pop_back();
	}
	std::cout << "Pool capacity: " << pool.available() << '/' << pool.capacity() << std::endl;

	return 0;
}

}

int main( void )
{
	try
	{
		return safemain();
	}
	catch ( const std::exception &e )
	{
		base::print_exception( std::cerr, e );
	}
}
