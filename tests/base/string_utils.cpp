//
// Copyright (c) 2015-2017 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include <base/contract.h>
#include <base/string_util.h>
#include <base/unit_test.h>
#include <base/scope_guard.h>
#include <base/cmd_line.h>
#include <base/language.h>
#include <iostream>

namespace
{

////////////////////////////////////////

int safemain( int argc, char *argv[] )
{
	base::unit_test strtest( "string" );

	base::cmd_line options( argv[0] );
	strtest.setup( options );

	auto errhandler = base::make_guard( [&]() { std::cerr << options << std::endl; } );
	options.parse( argc, argv );
	errhandler.dismiss();

	strtest["language"] = [&]( void )
	{
		try
		{
			std::string lang = base::locale::language();
			strtest.success( "language: " + lang );
		}
		catch ( std::exception &e )
		{
			strtest.failure( "unable to query language ({0})", e.what() );
		}

		try
		{
			base::locale::set( "en_US" );
			strtest.success( "set language to C" );
		}
		catch ( std::exception &e )
		{
			strtest.failure( "unable to set default locale ({0})", e.what() );
		}
	};

	strtest["trims"] = [&]( void )
	{
		strtest.test( base::trim( "\tfoo \n" ) == "foo", "base::trim" );
		strtest.test( base::trim( "foo \n" ) == "foo", "base::trim rightonly" );
		strtest.test( base::trim( " foo" ) == "foo", "base::trim leftonly" );
		strtest.test( base::trim( "foo" ) == "foo", "base::trim nochange" );
		strtest.test( base::ltrim( "\tfoo \n" ) == "foo \n", "base::ltrim" );
		strtest.test( base::ltrim( "foo \n" ) == "foo \n", "base::ltrim nochange" );
		strtest.test( base::rtrim( "\tfoo \n" ) == "\tfoo", "base::rtrim" );
		strtest.test( base::rtrim( "\tfoo" ) == "\tfoo", "base::rtrim nochange" );
	};

	strtest["replace"] = [&]( void )
	{
		strtest.test( base::replace( ":bar", ':', "foo" ) == "foobar", "base::replace" );
		strtest.test( base::replace( "bar", ':', "foo" ) == "bar", "base::replace noreplace" );
	};

	strtest["to_string"] = [&]( void )
	{
		strtest.test( base::to_string( 1 ) == "1", "base::to_string" );
	};

	strtest["cstring"] = [&]( void )
	{
		static_assert( base::cstring( "foobar" ) == "foobar", "compile time constexpr check" );
		base::cstring x( ":foo" );
		strtest.test( base::cstring( "foobar" ) == "foobar", "base::cstring::operator==" );
		strtest.test( x.find_first_not_of( ':', 0 ) == 1, "base::cstring::find_first_not_of" );
		strtest.test( x.find_first_of( ':', 0 ) == 0, "base::cstring::find_first_of" );
		strtest.test( x.find_last_of( 'o' ) == 3, "base::cstring::find_last_of" );
		strtest.test( x.substr( 1, 3 ) == "foo", "base::cstring::substr" );
		strtest.test( x.compare( ":bar" ) == 1, "base::cstring::compare" );
		strtest.test( x.compare( ":foz" ) == -1, "base::cstring::compare" );
		strtest.test( x.compare( ":foo" ) == 0, "base::cstring::compare" );
		strtest.test( x.compare( base::cstring( ":foo" ) ) == 0, "base::cstring::compare" );
		strtest.test( x.compare( std::string( ":foo" ) ) == 0, "base::cstring::compare" );
	};

	strtest["begins_with"] = [&]( void )
	{
		strtest.test( base::begins_with( base::cstring( "foobar" ), "foo" ), "base::begins_with" );
		strtest.test( ! base::begins_with( base::cstring( "foobar" ), "baz" ), "base::begins_with negative" );
		strtest.test( base::begins_with( base::cstring( "foobar" ), "" ), "base::begins_with empty" );
		strtest.test( base::begins_with( std::string( "foobar" ), "foo" ), "base::begins_with std::string" );
		strtest.test( ! base::begins_with( std::string( "foobar" ), "baz" ), "base::begins_with negative std::string" );
		strtest.test( base::begins_with( std::string( "foobar" ), "" ), "base::begins_with empty std::string" );
	};

	strtest["ends_with"] = [&]( void )
	{
		strtest.test( base::ends_with( base::cstring( "foobar" ), "bar" ), "base::ends_with" );
		strtest.test( ! base::ends_with( base::cstring( "foobar" ), base::cstring( "baz" ) ), "base::ends_with negative" );
		strtest.test( base::ends_with( base::cstring( "foobar" ), "" ), "base::ends_with empty std::string" );
		strtest.test( base::ends_with( std::string( "foobar" ), "bar" ), "base::ends_with std::string" );
		strtest.test( ! base::ends_with( std::string( "foobar" ), std::string( "baz" ) ), "base::ends_with negative std::string" );
		strtest.test( base::ends_with( std::string( "foobar" ), "" ), "base::ends_with empty std::string" );
	};

	strtest["length"] = [&]( void )
	{
		strtest.test( base::length( "foobar" ) == 6, "base::length" );
	};

	strtest["collate"] = [&]( void )
	{
		strtest.test( base::collate( base::cstring( "a" ), "b" ) == -1, "base::collate" );
		strtest.test( base::collate( base::cstring( "a" ), "a" ) == 0, "base::collate" );
		strtest.test( base::collate( base::cstring( "b" ), "a" ) == 1, "base::collate" );
		strtest.test( base::collate( base::wcstring( L"\u00e4ngel" ), L"\u00e5r", std::locale::classic() ) == -1, "base::collate unicode" );
	};

	strtest["icollate"] = [&]( void )
	{
		strtest.test( base::icollate( std::string( "A" ), "b" ) == -1, "base::icollate" );
		strtest.test( base::icollate( std::string( "a" ), "A" ) == 0, "base::icollate" );
		strtest.test( base::icollate( std::string( "b" ), "A" ) == 1, "base::icollate" );
//		strtest.test( base::icollate( std::wstring( L"ar" ), L"\u00e5r", std::locale::classic() ) == -1, "base::icollate unicode" );
	};

	strtest["split"] = [&]( void )
	{
		strtest.test( base::split( base::cstring( ":foo:" ), ':' ).size() == 3, "base::split( cstring ':foo:' on ':' (no skip empty) == 3" );
		strtest.test( base::split( base::cstring( ":foo:" ), ':', true ).size() == 1, "base::split( cstring ':foo:' on ':' (skip empty) == 1" );
	};

	strtest["to_upper"] = [&]( void )
	{
		strtest.test( base::to_upper( std::string( "foo" ) ) == "FOO", "base::to_upper" );
		strtest.test( base::to_upper( std::string( "FOO" ) ) == "FOO", "base::to_upper" );
		strtest.test( base::to_upper( std::string( "ooF" ) ) == "OOF", "base::to_upper" );
		strtest.test( base::to_upper( std::wstring( L"Zoë" ), base::locale::query( "en_US.UTF-8" ) ) == L"ZOË", "base::to_upper" );
	};

	strtest["to_lower"] = [&]( void )
	{
		strtest.test( base::to_lower( std::string( "foo" ) ) == "foo", "base::to_upper" );
		strtest.test( base::to_lower( std::string( "FOO" ) ) == "foo", "base::to_upper" );
		strtest.test( base::to_lower( std::string( "ooF" ) ) == "oof", "base::to_upper" );
		strtest.test( base::to_lower( std::wstring( L"Zoë" ), base::locale::query( "en_US.UTF-8" ) ) == L"zoë", "base::to_upper" );
	};
	strtest.run( options );
	strtest.clean();

	return - static_cast<int>( strtest.failure_count() );
}


////////////////////////////////////////


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
