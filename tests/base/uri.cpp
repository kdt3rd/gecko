//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include <base/unit_test.h>
#include <base/uri.h>
#include <base/contract.h>
#include <base/string_util.h>
#include <base/ansi.h>
#include <sstream>
#include <iostream>

namespace
{

void check( base::unit_test &ut, const std::string &test, bool should = true, bool except = false )
{
	try
	{
		base::uri u( test );
		std::string match = base::to_string( u );

		if ( test == match )
			ut.success( test );
		else if ( !should )
			ut.negative_success( test + " != " + match );
		else
			ut.failure( test + " != " + match );
	}
	catch ( const std::exception &e )
	{
		if ( except )
			ut.negative_success( test + " -> exception " + std::string( e.what() ) );
		else
			ut.failure( test + " -> exception " + std::string( e.what() ) );
		base::print_exception( std::cerr, e );
	}
}

int safemain( int argc, char *argv[] )
{
	base::unit_test test( "uri" );

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
	
	test["basic_parsing"] = [&]( void )
	{
		check( test, "http", false, true );
		check( test, "http:", false, true );
		check( test, "file:/tmp" );
		check( test, "file:///tmp", false );
		check( test, "http://host.com" );
		check( test, "http://user@host.com" );
		check( test, "http://host.com:1234" );
		check( test, "http://user@host.com:1234" );
		check( test, "http://host.com/path1" );
		check( test, "http://host.com/path1/path2" );
	}
	
	test["parse_query_frag"] = [&]( void )
	{
		check( test, "http://host.com?query" );
		check( test, "http://host.com#frag" );
		check( test, "http://host.com?query#frag" );
		check( test, "http://host.com/path1?query#frag" );
	}

	test["parse_encoded"] = [&]( void )
	{
		check( test, "http://host.com/path1#frag%3Fmorefrag" );
		check( test, "http://host%2ecom", false );
		check( test, "http://host%2Ecom", false );
		check( test, "http://host.com/%2fmore", false );
		check( test, "http://host.com/%2Fmore" );
		check( test, "http://host.com/with%20space", false );
		check( test, "http://host.com/with space", true );
	}

	test["path_construction"] = [&]( void )
	{
		base::uri tf( "file", "" );
		tf /= "file.txt";
		if ( tf.pretty() == "file:///file.txt" )
			test.success( "operator/=" );
		else
			test.failure( "operator/= -> {0}", tf );

		tf = base::uri( "file", "" ) / "hello" / "world";
		if ( tf.pretty() == "file:///hello/world" )
			test.success( "operator/" );
		else
			test.failure( "operator/ -> {0}", tf );
	}

//	test.cleanup() = [&]( void ) {};
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
