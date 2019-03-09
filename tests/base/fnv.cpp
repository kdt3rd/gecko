// Copyright (c) 2016-2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#include <base/contract.h>
#include <base/fnv_hash.h>
#include <base/unit_test.h>


////////////////////////////////////////

namespace
{

class Random
{
private:
	uint64_t _a, _b, _c, _d;
	static inline constexpr uint64_t rot64( uint64_t x, int k )
	{
		return ( x << k ) | ( x >> ( 64 - k ) );
	}

public:
	inline uint64_t value( void )
	{
		uint64_t e = _a - rot64( _b, 23 );
		_a = _b ^ rot64( _c, 16 );
		_b = _c + rot64( _d, 11 );
		_c = _d + e;
		_d = e + _a;
		return _d;
	}

	inline void init( uint64_t seed )
	{
		_a = 0xdeadbeef;
		_b = _c = _d = seed;
		for ( int i = 0; i != 20; ++i )
			value();
	}
};

int safemain( int argc, char *argv[] )
{
	base::unit_test test( "fnv_hash" );

	base::cmd_line options( argv[0] );
	test.setup( options );

	try
	{
		options.parse( argc, argv );
	}
	catch ( ... )
	{
		throw_add( "parsing command line arguments" );
	}

	test["smoke_32"] = [&]( void )
	{
		float x = 3.14F;
		base::fnv1a_32 h;
		h << x << "foobar";
		std::stringstream strb;
		strb << "hash: " << h();
		std::cout << strb.str() << std::endl;
	};

	test["smoke_64"] = [&]( void )
	{
		float x = 3.14F;
		base::fnv1a_64 h;
		h << x << "foobar";
		std::stringstream strb;
		strb << "hash: " << h();
		std::cout << strb.str() << std::endl;
	};

	test.run( options );
	test.clean();

	return - static_cast<int>( test.failure_count() );
}

}

int main( int argc, char *argv[] )
{
	try
	{
		return safemain( argc, argv );
	}
	catch ( const std::exception &e )
	{
		base::print_exception( std::cerr, e );
	}
	return -1;
}




