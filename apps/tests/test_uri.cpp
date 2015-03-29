
#include <base/uri.h>
#include <base/contract.h>
#include <base/string_util.h>
#include <base/ansi.h>
#include <sstream>
#include <iostream>

namespace
{

void check( const std::string &test, bool should = true, bool except = false )
{
	try
	{
		base::uri u( test );
		std::string match = base::to_string( u );

		if ( test == match )
			std::cout << base::ansi::green << "PASSED " << base::ansi::reset << test << std::endl;
		else if ( !should )
			std::cout << base::ansi::green << "FAILED " << base::ansi::reset <<  test << " != " << match << std::endl;
		else
			std::cout << base::ansi::red << "FAILED " << base::ansi::reset <<  test << " != " << match << std::endl;
	}
	catch ( const std::exception &e )
	{
		if ( except )
			std::cerr << base::ansi::green << "EXCEPTION " << base::ansi::reset << test << std::endl;
		else
			std::cerr << base::ansi::red << "EXCEPTION " << base::ansi::reset << test << std::endl;
		base::print_exception( std::cerr, e );
		std::cerr << base::ansi::reset << std::endl;
	}
}

int safemain( void )
{
	check( "http", false, true );
	check( "http:", false, true );
	check( "file:/tmp" );
	check( "file:///tmp", false );
	check( "http://host.com" );
	check( "http://user@host.com" );
	check( "http://host.com:1234" );
	check( "http://user@host.com:1234" );
	check( "http://host.com/path1" );
	check( "http://host.com/path1/path2" );

	check( "http://host.com?query" );
	check( "http://host.com#frag" );
	check( "http://host.com?query#frag" );
	check( "http://host.com/path1?query#frag" );
	check( "http://host.com/path1#frag\%3Fmorefrag" );

	check( "http://host\%2ecom", false );
	check( "http://host\%2Ecom", false );
	check( "http://host.com/\%2fmore", false );
	check( "http://host.com/\%2Fmore" );
	check( "http://host.com/with\%20space", false );
	check( "http://host.com/with space", true );

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
