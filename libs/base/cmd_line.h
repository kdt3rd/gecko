
#pragma once

#include <functional>
#include <vector>
#include <string>
#include "contract.h"

namespace base
{

////////////////////////////////////////

class cmd_line
{
public:
	class option
	{
	public:
		typedef std::function<bool(option&,size_t&,const std::vector<std::string>&)> callback;

		option( char s, const char *l, const char *a, const callback &c, const char *msg );
		option( char s, const std::string &l, const std::string &a, const callback &c, const std::string &msg );

		const std::string &long_name( void ) const
		{
			return _long;
		}

		char short_name( void ) const
		{
			return _short;
		}

		std::string name( void ) const
		{
			if ( _long.empty() )
			{
				if ( _short )
					return std::string( &_short, 1 );
				return std::string( "<unnamed>" );
			}
			return _long;

		}

		const std::string &args( void ) const
		{
			return _args;
		}

		const std::string &help( void ) const
		{
			return _help;
		}

		explicit operator bool( void ) const
		{
			return _has_value;
		}

		const std::string &value( void ) const
		{
			precondition( _values.size() == 1, "only 1 argument required" );
			return _values[0];
		}

		const std::vector<std::string> &values( void ) const
		{
			return _values;
		}

		void set( void )
		{
			_has_value = true;
		}

		void set_value( const std::string &v )
		{
			if ( _values.empty() )
				_values.push_back( v );
			else
				_values.front() = v;
			_has_value = true;
		}

		void add_value( std::string v )
		{
			_values.emplace_back( std::move( v ) );
			_has_value = true;
		}

		bool call( size_t &idx, const std::vector<std::string> &args )
		{
			return _callback( *this, idx, args );
		}

		bool match( const std::string &o );
		bool is_non_option( void );

	private:
		std::string _long;
		std::string _help;
		std::string _args;
		std::vector<std::string> _values;
		callback _callback;
		char _short = '\0';
		bool _has_value = false;
	};

	template<typename Range>
	cmd_line( const Range &opts )
		: _options( std::begin( opts ), std::end( opts ) )
	{
	}

	template<typename ...Opts>
	cmd_line( const option &o, Opts ...opts )
	{
		add( o, opts... );
	}

	template<typename ...Opts>
	void add( const option &o, Opts ...opts )
	{
		_options.push_back( o );
		add( opts... );
	}

	void add( const option &o )
	{
		_options.push_back( o );
	}

	const option &operator[]( size_t idx ) const
	{
		return _options.at( idx );
	}

	option &operator[]( size_t idx )
	{
		return _options.at( idx );
	}

	size_t size( void ) const
	{
		return _options.size();
	}

	const std::vector<option> &options( void ) const
	{
		return _options;
	}

	void parse( int argc, char *argv[] );
	void parse( const std::vector<std::string> &opts );

	static bool opt_none( option &opt, size_t &idx, const std::vector<std::string> &args );
	static bool opt_one( option &opt, size_t &idx, const std::vector<std::string> &args );
	static bool opt_optional( option &opt, size_t &idx, const std::vector<std::string> &args );
	static bool opt_many( option &opt, size_t &idx, const std::vector<std::string> &args );

	template<size_t n>
	static bool opt( option &opt, size_t &idx, const std::vector<std::string> &args )
	{
		if ( opt )
			throw_runtime( "option '{0}' already specified", opt.name() );
		if ( !opt.is_non_option() )
			++idx;
		for ( size_t i = 0; i < n; ++i )
		{
			if ( idx < args.size() )
			{
				if ( args[idx].find( '-' ) == 0 )
					throw_runtime( "option '{0}' needs {1} values (got {2})", opt.name(), n, i );
				opt.add_value( args[idx] );
				++idx;
			}
			else
				throw_runtime( "option '{0}' needs {1} values (got {2})", opt.name(), n, i );
		}
		return true;
	}

private:
	std::vector<option> _options;
};

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const cmd_line &opts );

////////////////////////////////////////

}

