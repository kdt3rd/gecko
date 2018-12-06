//
// Copyright (c) 2016-2017 Ian Godin
// SPDX-License-Identifier: MIT
//

#include <base/contract.h>
#include <base/unit_test.h>

#include <platform/platform.h>
#include <platform/system.h>
#include <gl/api.h>
#include <gl/vertex_buffer_data.h>

#include <iostream>

namespace
{

int safemain( int argc, char *argv[] )
{
	base::cmd_line options( argv[0] );

	base::unit_test test( "buffer_data" );
	test.setup( options );

	options.add_help();
	options.parse( argc, argv );

	if ( options["help"] )
	{
		std::cerr << options << std::endl;
		return -1;
	}

	gl::vertex_buffer_data<float,gl::vec2,gl::vec3,gl::vec4> tmp;

	test["stride"] = [&]( void )
	{
		test.test( tmp.stride() == 10, "stride is {0}", tmp.stride() );
	};

	test["components"] = [&]( void )
	{
		test.test( tmp.attributes() == 4, "components is {0}", tmp.attributes() );
	};

	test["offset 0"] = [&]( void )
	{
		size_t off = tmp.attribute_offset( 0 );
		test.test( off == 0, "offset 0 is {0}", off );
	};

	test["offset 1"] = [&]( void )
	{
		size_t off = tmp.attribute_offset( 1 );
		test.test( off == 1, "offset 1 is {0}", off );
	};

	test["offset 2"] = [&]( void )
	{
		size_t off = tmp.attribute_offset( 2 );
		test.test( off == 3, "offset 2 is {0}", off );
	};

	test["offset 3"] = [&]( void )
	{
		size_t off = tmp.attribute_offset( 3 );
		test.test( off == 6, "offset 3 is {0}", off );
	};

	test["push_back"] = [&]( void )
	{
		size_t size_before = tmp.size();
		tmp.push_back( 1.F, { 2.F, 3.F }, { 4.F, 5.F, 6.F }, { 7.F, 8.F, 9.F, 10.F } );
		size_t size_after = tmp.size();
		tmp.pop_back();
		size_t size_back = tmp.size();

		test.test( ( size_before + 10 == size_after ) && ( size_before == size_back ), "resizing: {0} -> {1} -> {2}", size_before, size_after, size_back );
	};

	test["initializer"] = [&]( void )
	{
		gl::vec2 x = { 1.F, 2.F };

		gl::vertex_buffer_data<gl::vec2,gl::vec2> data( {
			{ gl::vec2( 1.F, 2.F ), gl::vec2( 3.F, 4.F ) },
			{ gl::vec2( 5.F, 6.F ), gl::vec2( 7.F, 8.F ) }
		} );
		test.test( data.size() == 8, "data size: {0}", data.size() );
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
		std::cerr << '\n';
		base::print_exception( std::cerr, e );
	}
}
