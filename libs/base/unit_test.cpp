
#include "unit_test.h"
#include "ansi.h"
#include "contract.h"
#include "scope_guard.h"
#include "reverse.h"
#include <algorithm>

namespace base
{

////////////////////////////////////////

unit_test::unit_test( const std::string &n )
	: _name( n )
{
	precondition( !_name.empty(), "unit test without name" );
}

////////////////////////////////////////

unit_test::~unit_test( void )
{
}

////////////////////////////////////////

void unit_test::setup( cmd_line &opts )
{
	opts.add( cmd_line::option( 't', "test_" + _name, "<test> ...", cmd_line::args, "List of " + _name + " tests to run" ));
}

////////////////////////////////////////

void unit_test::success( const std::string &msg )
{
	std::clog << ansi::green << " SUCCESS " << ansi::reset << _running.back() << ": " << msg << std::endl;
}

////////////////////////////////////////

void unit_test::failure( const std::string &msg )
{
	std::clog << ansi::red << " FAILURE " << ansi::reset << _running.back() << ": " << msg << std::endl;
}

////////////////////////////////////////

void unit_test::warning( const std::string &msg )
{
	std::clog << ansi::yellow << " WARNING " << ansi::reset << _running.back() << ": " << msg << std::endl;
}

////////////////////////////////////////

void unit_test::message( const std::string &msg )
{
	std::clog << ansi::blue << " MESSAGE " << ansi::reset << _running.back() << ": " << msg << std::endl;
}

////////////////////////////////////////

void unit_test::run( void )
{
	for ( auto i = _tests.begin(); i != _tests.end(); ++i )
		run( i );
}

////////////////////////////////////////

void unit_test::run( cmd_line &opts )
{
	std::string optname = "test_" + _name;
	bool done = false;

	if ( auto &opt = opts[optname.c_str()] )
	{
		if ( !opt.values().empty() )
		{
			for ( auto &n: opt.values() )
				run( std::string( n ) );
			done = true;
		}
	}

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

void unit_test::run( std::map<std::string,std::function<void(void)>>::iterator &it )
{
	precondition( it != _tests.end(), "invalid test" );

	// Make sure we are not already running...
	const std::string &n = it->first;
	if ( std::find( _running.begin(), _running.end(), n ) == _running.end() )
		_running.emplace_back( n );
	else
		throw_runtime( "recursive tests not allowed: {0}", infix_separated( ", ", _running ) );
	on_scope_exit { _running.erase( std::remove( _running.begin(), _running.end(), n ), _running.end() ); };

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
		{
		}
	}
}

////////////////////////////////////////

}

