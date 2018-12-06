//
// Copyright (c) 2015-2017 Ian Godin
// SPDX-License-Identifier: MIT
//

#include <web/json_rpc.h>
#include <base/unit_test.h>

#include <sstream>
#include <iostream>
#include <unistd.h>

namespace
{


int safemain( int argc, char *argv[] )
{
	base::cmd_line options( argv[0] );

	base::unit_test test( "json_rpc" );
	test.setup( options );

	options.add_help();

	try
	{
		options.parse( argc, argv );
	}
	catch ( std::exception & )
	{
		std::cerr << options << std::endl;
		throw_add( "parsing command line" );
	}

	web::json_rpc server;
	server.method<void(const std::string &,int64_t,bool)>( "print_param" ) = [&]( const std::string &x, int64_t y, bool z ) -> void
	{
		test.message( "print_param {0} {1} {2}", x, y, z ? "true" : "false" );
	};

	server.method<base::json(void)>( "string_message" ) = []( void ) -> base::json
	{
		return base::json("Hello World");
	};

	test["void_call"] = [&]( void )
	{
		base::json params;
		params.push_back( "Hello World" );
		params.push_back( 42 );
		params.push_back( false );

		base::json rpc;
		rpc["jsonrpc"] = std::string( "2.0" );
		rpc["method"] = std::string( "print_param" );
		rpc["params"] = std::move( params );
		rpc["id"] = base::json( 1 );

		test.message( "return {0}", server.local_call( rpc ) );
		test.success( "void_call successful" );
	};

	test["ret_call"] = [&]( void )
	{
		base::json rpc;
		rpc["jsonrpc"] = "2.0";
		rpc["method"] = "string_message";
		rpc["id"] = 2;
		test.message( "return {0}", server.local_call( rpc )["result"] );
		test.success( "ret_call successful" );
	};

	test.run( options );
	test.clean();

	return -static_cast<int>( test.failure_count() );
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

