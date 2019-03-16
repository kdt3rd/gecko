// Copyright (c) 2015-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include <base/ansi.h>
#include <base/contract.h>
#include <base/string_util.h>
#include <base/unit_test.h>
#include <base/uri.h>
#include <iostream>
#include <sstream>

namespace
{

void check(
    base::unit_test &ut,
    const std::string &test,
    bool should = true,
    bool except = false )
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
        std::stringstream msg;
        base::print_exception( msg, e );
        if ( except )
            ut.negative_success( test + " -> exception " + msg.str() );
        else
            ut.failure( test + " -> exception " + msg.str() );
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

    test["basic_parsing"] = [&]( void ) {
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
    };

    test["parse_query_frag"] = [&]( void ) {
        check( test, "http://host.com?query" );
        check( test, "http://host.com#frag" );
        check( test, "http://host.com?query#frag" );
        check( test, "http://host.com/path1?query#frag" );
    };

    test["parse_encoded"] = [&]( void ) {
        check( test, "http://host.com/path1#frag%3Fmorefrag" );
        check( test, "http://host%2ecom", false );
        check( test, "http://host%2Ecom", false );
        check( test, "http://host.com/%2fmore", false );
        check( test, "http://host.com/%2Fmore" );
        check( test, "http://host.com/with%20space", false );
        check( test, "http://host.com/with space", true );
    };

    test["path_construction"] = [&]( void ) {
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
    };

    //	test.cleanup() = [&]( void ) {};
    test.run( options );
    test.clean();

    return -static_cast<int>( test.failure_count() );
}

} // namespace

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
