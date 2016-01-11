
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
///
/// Given a list of options, it will parse command line parameters and accumulate all values into each options.
/// Any errors will (e.g. unknown or mal-formed parameters) will trigger an exception.
///
/// example base/ex_cmd_line.cpp
class cmd_line
{
public:
	/// @brief Describes an option
	class option
	{
	public:
		typedef std::function<bool(option&,size_t&,const std::vector<char *>&)> callback;

		/// @brief Constructor (using const char *)
		/// @param s Short option name (single letter).
		/// @param l Long option name.
		/// @param a The name of the argument.
		/// @param c Callback to parse the value.
		/// @param msg Help message (usually shown with --help).
		/// @param required An exception is thrown is any required options are missing.
		option( char s, const char *l, const char *a, const callback &c, const char *msg, bool required = false );

		/// @brief Constructor (using std::string)
		/// @param s Short option name (single letter).
		/// @param l Long option name.
		/// @param a The name of the argument.
		/// @param c Callback to parse the value.
		/// @param msg Help message (usually shown with --help).
		/// @param required An exception is thrown is any required options are missing.
		option( char s, const std::string &l, const std::string &a, const callback &c, const std::string &msg, bool required = false );

		/// @brief Get the long name of the option.
		const std::string &long_name( void ) const
		{
			return _long;
		}

		/// @brief Get the short name of the option.
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
		const std::string &help( void ) const
		{
			return _help;
		}

		/// @brief Check if the option has been set.
		explicit operator bool( void ) const
		{
			return _has_value;
		}

		/// @brief Returns the option value (assuming a single value).
		char *value( void ) const
		{
			precondition( _values.size() == 1, "only 1 argument required" );
			return _values[0];
		}

		/// @brief Returns the list of values for this option.
		const std::vector<char *> &values( void ) const
		{
			return _values;
		}

		/// @brief Get the ith value.
		char *operator[]( size_t i )
		{
			return _values.at( i );
		}

		bool empty( void ) const
		{
			return _values.empty();
		}

		/// @brief The number of values this option has.
		size_t size( void ) const
		{
			return _values.size();
		}

		void clear( void )
		{
			_values.clear();
		}

		/// @brief Mark this option has having been set.
		void set( void )
		{
			_has_value = true;
		}

		/// @brief Check if this option is required.
		bool required( void ) const
		{
			return _required;
		}

		/// @brief Set the value of this option (assuming a single value).
		void set_value( char *v )
		{
			if ( _values.empty() )
				_values.push_back( v );
			else
				_values.front() = v;
			_has_value = true;
		}

		/// @brief Add a value to this option.
		void add_value( char *v )
		{
			_values.push_back( v );
			_has_value = true;
		}

		/// @brief Run the callback on the idx-th item in args.
		bool call( size_t &idx, const std::vector<char *> &args )
		{
			return _callback( *this, idx, args );
		}

		/// @brief Check if o matches this option.
		bool match( const std::string &o );

		/// @brief Check if this is not really an option (short/long name empty).
		bool is_non_option( void );

	private:
		std::string _long;
		std::string _help;
		std::string _args;
		std::vector<char *> _values;
		callback _callback;
		char _short = '\0';
		bool _has_value = false;
		bool _required = false;
	};

	/// @brief Constructor
	/// No options are created, but can be added.
	cmd_line( const char *prog )
		: _program( prog )
	{
		precondition( prog != nullptr, "null program name" );
	}

	/// @brief Constructor with list of options.
	template<typename Range>
	cmd_line( const char *prog, const Range &opts )
		: _program( prog ), _options( std::begin( opts ), std::end( opts ) )
	{
		precondition( prog != nullptr, "null program name" );
	}

	/// @brief Constructor with list of options.
	template<typename ...Opts>
	cmd_line( const char *prog, const option &o, Opts ...opts )
		: _program( prog )
	{
		precondition( prog != nullptr, "null program name" );
		add( o, opts... );
	}

	/// @brief Add several options
	template<typename ...Opts>
	void add( const option &o, Opts ...opts )
	{
		_options.push_back( o );
		add( opts... );
	}

	/// @brief Add one option.
	void add( const option &o )
	{
		_options.push_back( o );
	}

	/// @brief Add standard help option
	/// This will add an option for -h and -help.
	/// The option will cause the program to print a help messages and exit.
	void add_help( void );

	/// @brief Get nth option.
	const option &operator[]( size_t n ) const
	{
		return _options.at( n );
	}

	/// @brief Get nth option.
	option &operator[]( size_t n )
	{
		return _options.at( n );
	}

	/// @brief Get option by name.
	const option &operator[]( const char *n ) const;

	/// @brief Get option by name.
	option &operator[]( const char *n );

	/// @brief Get number of options.
	size_t size( void ) const
	{
		return _options.size();
	}

	/// @brief Return the vector of options.
	const std::vector<option> &options( void ) const
	{
		return _options;
	}

	/// @brief Parse options.
	void parse( int argc, char *argv[] );
	void parse( const std::vector<char *> &args );

	/// @brief Simple usage message
	std::string simple_usage( void ) const;

	/// @name Common callback
	/// These are common callbacks used to construct options.
	/// @{
	/// @brief Handle argument requiring "n" strings
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

	/// @brief Handle argument requiring between "a" and "b" strings
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

	/// @brief Handle a flag (with no values)
	static bool flag( option &opt, size_t &idx, const std::vector<char *> &args );

	/// @brief Handle a list of values
	/// This will grab all values until the end, or a string starting with "-" is encountered.
	static bool args( option &opt, size_t &idx, const std::vector<char *> &args );

	/// @brief Handle one value added to a list option.
	static bool multi( option &opt, size_t &idx, const std::vector<char *> &args );

	/// @brief Handle options that are counted, but do not have any values.
	/// For example, using "-v -v -v" for extra verbose.
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

