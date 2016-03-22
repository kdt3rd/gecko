
#pragma once

#include <functional>
#include <vector>
#include <string>
#include <limits>
#include "contract.h"

namespace base
{

////////////////////////////////////////

/// @brief Command-line parameters parser.
/// Given a list of options, it will parse command line parameters and accumulate all values into each options.
/// Any errors will (e.g. unknown or mal-formed parameters) will trigger an exception.
/// @example base/cmd_line.cpp
class cmd_line
{
public:
	/// @brief Describes an option.
	/// Option which is to be parsed as part of the command line.
	class option
	{
	public:
		/// @brief Function to parse the command line arguments for an option.
		/// Parser functions are used to process the arguments strings.
		/// They are given a list of strings, along with an index to start parsing at.
		/// A parser should use as many strings as needed, adding values to the option.
		/// It should also increment the index value to match the number of strings used.
		/// Finally, true or false is returned to indicate success.
		typedef std::function<bool(option&,size_t&,const std::vector<char *>&)> parser;

		/// @brief Constructor (using const char *).
		/// @param s Short option name (single letter).
		/// @param l Long option name.
		/// @param a The name of the argument.
		/// @param p Parser function for the value.
		/// @param msg Help message (usually shown with --help).
		/// @param required An exception is thrown is any required options are missing.
		option( char s, const char *l, const char *a, const parser &p, const char *msg, bool required = false );

		/// @brief Constructor (using std::string).
		/// @param s Short option name (single letter).
		/// @param l Long option name.
		/// @param a The name of the argument.
		/// @param p Parser function for the value.
		/// @param msg Help message (usually shown with --help).
		/// @param required An exception is thrown is any required options are missing.
		option( char s, const std::string &l, const std::string &a, const parser &p, const std::string &msg, bool required = false );

		/// @brief Get the long name of the option.
		/// @returns The long name for the option.
		const std::string &long_name( void ) const
		{
			return _long;
		}

		/// @brief Get the short name of the option.
		/// @returns The short name (single character) for the option.
		char short_name( void ) const
		{
			return _short;
		}

		/// @brief Creates a name for the option.
		/// The long name is used, if available.
		/// Otherwise the short name is used.
		/// If the short name is 0, the argument name is used.
		/// Finally, if there is nothing, <unnamed> is used.
		std::string name( void ) const;

		/// @brief Argument name.
		/// This is shown in help messages.
		/// It is also used as a name, if no short/long name is given.
		const std::string &args_help( void ) const
		{
			return _args;
		}

		/// @brief The help message.
		/// @returns The help message.
		const std::string &help( void ) const
		{
			return _help;
		}

		/// @brief Check if the option has been set.
		/// @returns true if the option is set.
		explicit operator bool( void ) const
		{
			return _has_value;
		}

		/// @brief Returns the option value (assuming a single value).
		/// The option must have a single value.
		/// @returns The value for the option.
		char *value( void ) const
		{
			precondition( _values.size() == 1, "only 1 argument required" );
			return _values[0];
		}

		/// @brief Returns the list of values for this option.
		/// @returns The list of values set on the command line for this option.
		const std::vector<char *> &values( void ) const
		{
			return _values;
		}

		/// @brief Get the ith value.
		/// @param i The index of the value to get.
		/// @returns The ith value for this option.
		char *operator[]( size_t i )
		{
			return _values.at( i );
		}

		/// @brief Check is this option has any values.
		/// @returns True if the option has no values.
		bool empty( void ) const
		{
			return _values.empty();
		}

		/// @brief The number of values.
		/// @returns The number of values contained in this option.
		size_t size( void ) const
		{
			return _values.size();
		}

		/// @brief Removes all values from the option.
		void clear( void )
		{
			_values.clear();
		}

		/// @brief Mark this option has having been set.
		/// This is useful if the option has no values, but is still set (option flags).
		void set( void )
		{
			_has_value = true;
		}

		/// @brief Check if this option is required.
		/// @returns True is this option is required.
		bool required( void ) const
		{
			return _required;
		}

		/// @brief Set the value of this option.
		/// Assumes this option uses a single value.
		/// This will overwrite any previous value set.
		/// @param v Value to set.
		void set_value( char *v )
		{
			if ( _values.empty() )
				_values.push_back( v );
			else
				_values.front() = v;
			_has_value = true;
		}

		/// @brief Add a value to this option.
		/// Multiple values can be added to this option.
		/// @param v Value to add.
		void add_value( char *v )
		{
			_values.push_back( v );
			_has_value = true;
		}

		/// @brief Run the parser on the idx-th item in args.
		/// @param idx Index to start parsing at.
		/// @param args Arguments to parse.
		/// @returns True if parsing was successful.
		bool call( size_t &idx, const std::vector<char *> &args )
		{
			return _parser( *this, idx, args );
		}

		/// @brief Check if o matches this option.
		/// @param o See if "o" matches the name of this option.
		/// @returns True if it matches.
		bool match( const std::string &o );

		/// @brief Check if this is not really an option (short/long name empty).
		/// @returns True if not an option.
		bool is_non_option( void );

	private:
		std::string _long;
		std::string _help;
		std::string _args;
		std::vector<char *> _values;
		parser _parser;
		char _short = '\0';
		bool _has_value = false;
		bool _required = false;
	};

	/// @name Constructor
	/// @{

	/// @brief Simple constructor.
	/// No options are created, but can be added.
	/// @param prog Program name to use when printing out option help.
	cmd_line( const char *prog )
	{
		precondition( prog != nullptr, "null program name" );
		_program = prog;
	}

	/// @brief Constructor with list of options.
	/// @tparam Range Container type which can be used with range-base for loop.
	/// @param prog Program name to use when printing out option help.
	/// @param opts Container with options.
	template<typename Range>
	cmd_line( const char *prog, const Range &opts )
		: _options( std::begin( opts ), std::end( opts ) )
	{
		precondition( prog != nullptr, "null program name" );
		_program = prog;
	}

	/// @brief Constructor with one or more option.
	/// @tparam Opts Variadic type which should be options.
	/// @param prog Program name to use when printing out option help.
	/// @param o Option to add to the command line parser.
	/// @param opts Extra options to also add.
	template<typename ...Opts>
	cmd_line( const char *prog, const option &o, Opts ...opts )
		: _program( prog )
	{
		precondition( prog != nullptr, "null program name" );
		add( o, opts... );
	}

	/// @}

	/// @name Modifiers
	/// @{

	/// @brief Add one or more option.
	/// @tparam Opts Variadic type which should be options.
	/// @param o Option to add to the command line parser.
	/// @param opts Extra options to also add.
	template<typename ...Opts>
	void add( const option &o, Opts ...opts )
	{
		_options.push_back( o );
		add( opts... );
	}

	/// @brief Add an option.
	/// @param o Option to add to the command line parser.
	void add( const option &o )
	{
		_options.push_back( o );
	}

	/// @brief Add standard help option.
	/// This will add an option for -h and -help.
	/// The option will cause the program to print a help messages and exit.
	void add_help( void );

	/// @}

	/// @name Accessors
	/// @{

	/// @brief Get nth option.
	/// @param n Index of option to get.
	/// @returns The nth option.
	const option &operator[]( size_t n ) const
	{
		return _options.at( n );
	}

	/// @brief Get nth option.
	/// @param n Index of option to get.
	/// @returns The nth option.
	option &operator[]( size_t n )
	{
		return _options.at( n );
	}

	/// @brief Get option by name.
	/// @param n Name of option to get.
	/// @returns The named option.
	const option &operator[]( const char *n ) const;

	/// @brief Get option by name.
	/// @param n Name of option to get.
	/// @returns The named option.
	option &operator[]( const char *n );

	/// @brief Get number of options.
	/// @returns The number of options.
	size_t size( void ) const
	{
		return _options.size();
	}

	/// @brief Return the vector of options.
	/// @returns Vector of all options.
	const std::vector<option> &options( void ) const
	{
		return _options;
	}

	/// @brief Simple usage message.
	/// @returns String containing the message.
	std::string simple_usage( void ) const;

	/// @}

	/// @name Parser
	/// @{

	/// @brief Parse options from argc/argv.
	/// @param argc Number of arguments in argv.
	/// @param argv List of string arguments to parse.
	void parse( int argc, char *argv[] );

	/// @brief Parse options from a vector of strings.
	/// @param args Vector of string arguments to parse.
	void parse( const std::vector<char *> &args );

	/// @}

	/// @name Common Parsers
	/// @brief These are common parsers used to construct options.
	/// @see cmd_line::option
	/// @{

	/// @brief Handle argument requiring a fixed number of strings.
	/// A single argument is very common.
	/// @tparam n Number of strings expected.
	/// @param opt Option to process.
	/// @param idx Index in args to start processing.
	/// @param args List of arguments to process.
	/// @returns True if parsing was successful.
	template<size_t n>
	static bool arg( option &opt, size_t &idx, const std::vector<char *> &args )
	{
		if ( opt )
			return false;

		if ( !opt.is_non_option() )
			++idx;

		if ( n == 0 )
			opt.set();

		for ( size_t i = opt.size(); i < n; ++i )
		{
			if ( idx < args.size() )
			{
				if ( args[idx][0] == '-' )
					throw_runtime( "option '{0}' needs {1} values (got {2})", opt.name(), n, i );
				opt.add_value( args[idx] );
				++idx;
			}
			else
				throw_runtime( "option '{0}' needs {1} values (got {2})", opt.name(), n, i );
		}
		return true;
	}

	/// @brief Handle argument requiring between "a" and "b" strings.
	/// @tparam a Minimum number of arguments expected for this option.
	/// @tparam b Maximum number of arguments expected for this option.
	/// @param opt Option to process.
	/// @param idx Index in args to start processing.
	/// @param args List of arguments to process.
	/// @returns True if parsing was successful.
	template<size_t a,size_t b>
	static bool arg( option &opt, size_t &idx, const std::vector<char *> &args )
	{
		static_assert( a < b, "invalid argument range" );
		if ( opt )
			throw_runtime( "option '{0}' already specified", opt.name() );

		if ( !opt.is_non_option() )
			++idx;

		if ( a == 0 )
			opt.set();

		for ( size_t i = 0; i < b; ++i )
		{
			if ( idx < args.size() )
			{
				if ( args[idx][0] == '-' )
				{
					if ( i < a )
						throw_runtime( "option '{0}' needs at least {1} values (got {2})", opt.name(), a, i );
					break;
				}
				opt.add_value( args[idx] );
				++idx;
			}
			else
			{
				if ( i < a )
					throw_runtime( "option '{0}' needs at least {1} values (got {2})", opt.name(), a, i );
				break;
			}
		}

		return true;
	}

	/// @brief Handle a flag (with no values).
	/// @param opt Option to process.
	/// @param idx Index in args to start processing.
	/// @param args List of arguments to process.
	/// @returns True if parsing was successful.
	static bool flag( option &opt, size_t &idx, const std::vector<char *> &args );

	/// @brief Handle a list of values.
	/// This will grab all values until the end, or a string starting with "-" is encountered.
	/// @param opt Option to process.
	/// @param idx Index in args to start processing.
	/// @param args List of arguments to process.
	/// @returns True if parsing was successful.
	static bool args( option &opt, size_t &idx, const std::vector<char *> &args );

	/// @brief Handle one value added to a list option.
	/// @param opt Option to process.
	/// @param idx Index in args to start processing.
	/// @param args List of arguments to process.
	/// @returns True if parsing was successful.
	static bool multi( option &opt, size_t &idx, const std::vector<char *> &args );

	/// @brief Handle options that are counted, but do not have any values.
	/// For example, using "-v -v -v" for extra verbose.
	/// @param opt Option to process.
	/// @param idx Index in args to start processing.
	/// @param args List of arguments to process.
	/// @returns True if parsing was successful.
	static bool counted( option &opt, size_t &idx, const std::vector<char *> &args );

	/// @}

private:
	std::string _program;
	std::vector<option> _options;
};

////////////////////////////////////////

/// @brief Print help message
std::ostream &operator<<( std::ostream &out, const cmd_line &opts );

////////////////////////////////////////

}

