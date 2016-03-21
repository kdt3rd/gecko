
#pragma once

#include <string>
#include <functional>
#include <map>
#include <set>
#include "format.h"
#include "cmd_line.h"

namespace base
{

////////////////////////////////////////

/// @brief Unit test
///
/// A Unit test with a name and a list of functions.
/// If a function throws an exception, the test will have failed.
/// A function should use the test() to indicate the result of a test.
/// It can also call success() and failure() explicitly.
/// Extra messages can also be displayed with warning() and message().
class unit_test
{
public:
	/// @brief Constructor.
	/// @param n Name of the test.
	unit_test( const std::string &n );

	/// @brief Destructor.
	~unit_test( void );

	/// @brief Add testing option to the command line parser.
	/// @param opts Command line parser to add options to.
	void setup( cmd_line &opts );

	/// @brief Create a test named 'n'
	/// @param n Name of the new test.
	std::function<void(void)> &operator[]( const std::string &n )
	{
		return _tests[n];
	}

	/// @brief Indicate the result of a test.
	/// @param v True if the test succeeded, false otherwise.
	/// @param args Message to display (can use formatting).
	/// @sa base::format
	template<typename ...Args>
	void test( bool v, Args ...args )
	{
		if ( v )
			success( args... );
		else
			failure( args... );
	}

	void success( const std::string &msg );
	void failure( const std::string &msg );
	void warning( const std::string &msg );
	void message( const std::string &msg );

	template<typename ...Args>
	void success( Args ...args )
	{
		success( std::string( format( std::forward<Args>( args )... ) ) );
	}

	template<typename ...Args>
	void failure( Args ...args )
	{
		failure( std::string( format( std::forward<Args>( args )... ) ) );
	}

	template<typename ...Args>
	void warning( Args ...args )
	{
		warning( std::string( format( std::forward<Args>( args )... ) ) );
	}

	template<typename ...Args>
	void message( Args ...args )
	{
		message( std::string( format( std::forward<Args>( args )... ) ) );
	}

	void set_quiet( int level );

	void run( void );
	void run( const std::string &n );
	void run( cmd_line &opts );

	void clean( void );

	void summarize( void );

	size_t success_count( void ) const
	{
		return _success.size();
	}

	size_t failure_count( void ) const
	{
		return _failure.size();
	}

	size_t run_count( void ) const
	{
		return _success.size() + _failure.size();
	}

	std::function<void(void)> &cleanup( void )
	{
		_cleanup.emplace_back();
		return _cleanup.back();
	}

private:
	void run( std::map<std::string,std::function<void(void)>>::iterator &it );

	std::map<std::string,std::function<void(void)>> _tests;
	std::set<std::string> _success;
	std::set<std::string> _failure;
	std::vector<std::string> _running;

	std::vector<std::function<void(void)>> _cleanup;

	std::string _name;
	std::function<void(void)> _test;
	int _quiet = 0;
};

////////////////////////////////////////

}

