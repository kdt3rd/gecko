
#include "cmd_line.h"
#include <cstring>

namespace base
{

////////////////////////////////////////

cmd_line::option::option( char s, const char *l, const char *a, const callback &c, const char *msg, bool req )
	: _long( l ), _help( msg ), _args( a ), _callback( c ), _short( s ), _required( req )
{
}

////////////////////////////////////////

cmd_line::option::option( char s, const std::string &l, const std::string &a, const callback &c, const std::string &msg, bool req )
	: _long( l ), _help( msg ), _args( a ), _callback( c ), _short( s ), _required( req )
{
}

////////////////////////////////////////

std::string
cmd_line::option::name( void ) const
{
	if ( _long.empty() )
	{
		if ( _short != 0 )
			return std::string( &_short, 1 );
		if ( !_args.empty() )
			return _args;
		return std::string( "<unnamed>" );
	}
	return _long;
}

////////////////////////////////////////

bool
cmd_line::option::match( const std::string &o )
{
	if ( o.empty() )
		return false;

	if ( o[0] == '-' )
	{
		if ( o.size() == 2 )
		{
			if ( o[1] == _short )
				return true;
		}

		if ( o.size() > 2 && o[1] == '-' )
		{
			if ( o.compare( 2, std::string::npos, _long ) == 0 )
				return true;
		}
		else if ( o.compare( 1, std::string::npos, _long ) == 0 )
			return true;

		return false;
	}

	return is_non_option();
}

////////////////////////////////////////

bool
cmd_line::option::is_non_option( void )
{
	return _short == '\0' && _long.empty();
}

////////////////////////////////////////

void cmd_line::add_help( void )
{
	auto helpfunc = [=]( option &opt, size_t &i, const std::vector<char *> &vals ) -> bool
	{
		std::cout << *this << std::endl;
		exit( 0 );
		return true;
	};
	add( option( 'h', "help", std::string(), helpfunc, "Print help message and exit", false ) );
}

////////////////////////////////////////

const cmd_line::option &
cmd_line::operator[]( const char *n ) const
{
	for ( const auto &opt: _options )
	{
		if ( opt.name().compare( n ) == 0 )
			return opt;
	}
	throw_runtime( "option '{0}' not found", n );
}

////////////////////////////////////////

cmd_line::option &
cmd_line::operator[]( const char *n )
{
	for ( auto &opt: _options )
	{
		if ( opt.name().compare( n ) == 0 )
			return opt;
	}
	throw_runtime( "option '{0}' not found", n );
}

////////////////////////////////////////

void
cmd_line::parse( int argc, char *argv[] )
{
	precondition( argc > 0, "not enough arguments" );
	std::vector<char *> tmp( argv+1, argv+argc );
	parse( tmp );
}

////////////////////////////////////////

void
cmd_line::parse( const std::vector<char *> &args )
{
	size_t current = 0;

	while ( current < args.size() )
	{
		char *arg = args[current];

		if ( std::strcmp( arg, "--" ) == 0 )
		{
			++current;
			break;
		}

		// Try each option in order...
		bool matched = false;
		for ( auto &opt: _options )
		{
			if ( opt.match( arg ) )
			{
				matched = opt.call( current, args );
				if ( matched )
					break;
			}
		}

		if ( !matched )
			throw_runtime( "unknown option: {0}", arg );
	}

	// After the "--", no options allowed (only arguments)
	while ( current < args.size() )
	{
		char *arg = args[current];
		bool matched = false;
		for ( auto &opt: _options )
		{
			if ( opt.is_non_option() )
				matched = opt.call( current, args );
		}
		if ( !matched )
			throw_runtime( "extra argument: {0}", arg );
	}

	// Check for required options
	for ( auto &opt: _options )
	{
		if ( opt.required() && !opt )
			throw_runtime( "required option '{0}' missing", opt.name() );
	}
}

////////////////////////////////////////

std::string
cmd_line::simple_usage( void ) const
{
	std::stringstream result;
	result << _program;
	for ( auto &opt: _options )
	{
		if ( !opt.required() )
			result << " [";
		else
			result << " ";

		bool space = true;
		if ( opt.short_name() != 0 )
			result << "-" << opt.short_name();
		else if ( !opt.long_name().empty() )
			result << "--" << opt.long_name();
		else
			space = false;

		if ( !opt.args_help().empty() )
		{
			if ( space )
				result << ' ';
			result << opt.args_help();
		}
		if ( !opt.required() )
			result << "]";
	}
	return std::move( result.str() );
}

////////////////////////////////////////

bool cmd_line::flag( option &opt, size_t &idx, const std::vector<char *> &args )
{
	precondition( !opt.is_non_option(), "flag should be a normal option" );

	if ( opt )
		return false;

	++idx;

	opt.set();

	return true;
}

////////////////////////////////////////

bool
cmd_line::args( option &opt, size_t &idx, const std::vector<char *> &args )
{
	if ( !opt.is_non_option() )
		++idx;

	if ( idx >= args.size() || args[idx][0] == '-' )
		throw_runtime( "option '{0}' expected at least 1 value", opt.name() );

	while ( idx < args.size() )
	{
		if ( args[idx][0] == '-' )
			break;
		opt.add_value( args[idx] );
		++idx;
	}

	return true;
}

////////////////////////////////////////

bool
cmd_line::multi( option &opt, size_t &idx, const std::vector<char *> &args )
{
	if ( !opt.is_non_option() )
		++idx;

	if ( idx >= args.size() || args[idx][0] == '-' )
		throw_runtime( "option '{0}' expected at least 1 value", opt.name() );

	opt.add_value( args[idx] );
	++idx;

	return true;
}

////////////////////////////////////////

bool
cmd_line::counted( option &opt, size_t &idx, const std::vector<char *> &args )
{
	precondition( !opt.is_non_option(), "unnamed option not allowed as counted flag" );
	opt.add_value( nullptr );
	++idx;
	return true;
}

////////////////////////////////////////

std::ostream &
operator<<( std::ostream &out, const cmd_line &cmdline )
{
	out << "Usage: " << cmdline.simple_usage() << '\n';
	std::vector<std::string> list;
	size_t width = 0;
	for ( auto &opt: cmdline.options() )
	{
		std::string tmp;
		if ( opt.short_name() )
		{
			tmp.push_back( '-' );
			tmp.push_back( opt.short_name() );
		}
		if ( !opt.long_name().empty() )
		{
			if ( tmp.empty() )
				tmp = "--" + opt.long_name();
			else
				tmp += ", --" + opt.long_name();
		}

		if ( !opt.args_help().empty() )
		{
			if ( tmp.empty() )
				tmp = opt.args_help();
			else
				tmp += " " + opt.args_help();
		}
		width = std::max( width, tmp.size() );
		list.push_back( tmp );
	}

	size_t i = 0;
	for ( auto &opt: cmdline.options() )
		out << format( "  {0,w" + to_string( width ) + "}  {1}\n", list[i++], opt.help() );

	return out;
}

////////////////////////////////////////

}

