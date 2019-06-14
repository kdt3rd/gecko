// Copyright (c) 2014-2016 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include "unit_test.h"

#include "ansi.h"
#include "contract.h"
#include "reverse.h"
#include "scope_guard.h"
#include "string_split.h"

#include <algorithm>

namespace base
{
////////////////////////////////////////

unit_test::unit_test( const std::string &n ) : _name( n )
{
    precondition( !_name.empty(), "unit test without name" );
}

////////////////////////////////////////

unit_test::~unit_test( void ) {}

////////////////////////////////////////

void unit_test::setup( cmd_line &opts )
{
    opts.add( cmd_line::option(
        't',
        "test_" + _name,
        "<test> ...",
        cmd_line::args,
        "List of " + _name + " tests to run" ) );
    opts.add( cmd_line::option(
        'q',
        "",
        "",
        cmd_line::counted,
        "Quiet mode (can be specified multiple times)" ) );
}

////////////////////////////////////////

void unit_test::success( const std::string &msg )
{
    _success.insert( _running.back() );
    if ( _quiet < 1 )
        std::clog << ansi::green << " SUCCESS " << ansi::reset
                  << _running.back() << ": " << msg << std::endl;
}

////////////////////////////////////////

void unit_test::negative_success( const std::string &msg )
{
    _success.insert( _running.back() );
    if ( _quiet < 1 )
        std::clog << ansi::green << " FAILURE " << ansi::reset
                  << _running.back() << ": " << msg << std::endl;
}

////////////////////////////////////////

void unit_test::failure( const std::string &msg )
{
    _failure.insert( _running.back() );
    if ( _quiet < 2 )
        std::clog << ansi::red << " FAILURE " << ansi::reset << _running.back()
                  << ": " << msg << std::endl;
}

////////////////////////////////////////

void unit_test::warning( const std::string &msg )
{
    if ( _quiet < 2 )
        std::clog << ansi::yellow << " WARNING " << ansi::reset
                  << _running.back() << ": " << msg << std::endl;
}

////////////////////////////////////////

void unit_test::message( const std::string &msg )
{
    if ( _quiet < 1 )
        std::clog << ansi::blue << " MESSAGE " << ansi::reset << _running.back()
                  << ": " << msg << std::endl;
}

////////////////////////////////////////

void unit_test::set_quiet( int level ) { _quiet = level; }

////////////////////////////////////////

void unit_test::run( void )
{
    for ( auto i = _tests.begin(); i != _tests.end(); ++i )
        run( i );
    summarize();
}

////////////////////////////////////////

void unit_test::run( cmd_line &opts )
{
    std::string optname = "test_" + _name;
    bool        done    = false;

    if ( auto &opt = opts[optname.c_str()] )
    {
        if ( !opt.values().empty() )
        {
            for ( auto &n: opt.values() )
                run( std::string( n ) );
            done = true;
            summarize();
        }
    }

    if ( auto &q = opts["q"] )
        _quiet = static_cast<int>( q.size() );

    if ( !done )
        run();
}

////////////////////////////////////////

void unit_test::run( const std::string &n )
{
    auto it = _tests.find( n );
    if ( it == _tests.end() )
        throw_runtime( "test '{0}' not found", n );
    run( it );
}

////////////////////////////////////////

void unit_test::run(
    std::map<std::string, std::function<void( void )>>::iterator &it )
{
    precondition( it != _tests.end(), "invalid test" );

    // Make sure we are not already running...
    const std::string &n = it->first;
    if ( std::find( _running.begin(), _running.end(), n ) == _running.end() )
        _running.emplace_back( n );
    else
        throw_runtime(
            "recursive tests not allowed: {0}",
            infix_separated( ", ", _running ) );
    on_scope_exit
    {
        _running.erase(
            std::remove( _running.begin(), _running.end(), n ),
            _running.end() );
    };

    // Already completed
    if ( _success.find( n ) != _success.end() )
        return;
    if ( _failure.find( n ) != _failure.end() )
        throw_runtime( "test '{0}' has failed", n );

    // Run the test
    size_t failures = _failure.size();
    try
    {
        it->second();
    }
    catch ( std::exception &e )
    {
        std::stringstream tmp;
        base::print_exception( tmp, e );
        std::vector<std::string> lines;
        base::split( tmp.str(), '\n', std::back_inserter( lines ), true );
        for ( auto &l: lines )
            message( l );
        failure( std::string( e.what() ) );
    }

    if ( failures != _failure.size() )
        _failure.insert( n );
    else
        _success.insert( n );
}

////////////////////////////////////////

void unit_test::clean( void )
{
    for ( auto &c: base::reverse( _cleanup ) )
    {
        try
        {
            c();
        }
        catch ( ... )
        {}
    }
}

////////////////////////////////////////

void unit_test::summarize( void )
{
    if ( _quiet > 0 && _quiet < 3 )
    {
        if ( success_count() == run_count() )
            std::clog << ansi::green << " SUCCESS " << ansi::reset << _name
                      << ": " << run_count() << " / " << run_count()
                      << " passed" << std::endl;
        else
        {
            std::clog << ansi::red << " FAILURE " << ansi::reset << _name
                      << ": ( " << ansi::green << success_count() << ansi::reset
                      << ", " << ansi::red << failure_count() << ansi::reset
                      << " ) / " << run_count() << std::endl;
        }
    }
}

////////////////////////////////////////

} // namespace base
