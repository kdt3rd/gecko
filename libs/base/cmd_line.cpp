
#include "cmd_line.h"

namespace base
{

////////////////////////////////////////

cmd_line::option::option( char s, const char *l, const char *a, const callback &c, const char *msg )
	: _long( l ), _help( msg ), _args( a ), _callback( c ), _short( s )
{
}

////////////////////////////////////////

cmd_line::option::option( char s, const std::string &l, const std::string &a, const callback &c, const std::string &msg )
	: _long( l ), _help( msg ), _args( a ), _callback( c ), _short( s )
{
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

void
cmd_line::parse( int argc, char *argv[] )
{
	precondition( argc > 0, "not enough arguments" );
	std::vector<std::string> tmp( argv+1, argv+argc );
	parse( tmp );
}

////////////////////////////////////////

void
cmd_line::parse( const std::vector<std::string> &args )
{
	size_t current = 0;
	
	while ( current < args.size() )
	{
		const std::string &arg = args[current];

		if ( arg == "--" )
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

	while ( current < args.size() )
	{
		const std::string &arg = args[current];
		bool matched = false;
		for ( auto &opt: _options )
		{
			if ( opt.is_non_option() )
				matched = opt.call( current, args );
		}
		if ( !matched )
			throw_runtime( "unknown option: {0}", arg );
	}
}

////////////////////////////////////////

bool
cmd_line::opt_none( option &opt, size_t &idx, const std::vector<std::string> &args )
{
	++idx;
	opt.set();
	return true;
}

////////////////////////////////////////

bool
cmd_line::opt_one( option &opt, size_t &idx, const std::vector<std::string> &args )
{
	if ( !opt.is_non_option() )
		++idx;
	if ( idx < args.size() )
	{
		if ( args[idx].find( '-' ) == 0 )
			throw_runtime( "option '{0}' needs a value", opt.name() );
		if ( opt )
			return false;
		opt.set_value( args[idx] );
		++idx;
		return true;
	}
	else
		throw_runtime( "option '{0}' needs a value", opt.name() );
}

////////////////////////////////////////

bool
cmd_line::opt_optional( option &opt, size_t &idx, const std::vector<std::string> &args )
{
	if ( !opt.is_non_option() )
		++idx;
	if ( idx < args.size() )
	{
		if ( args[idx].find( '-' ) != 0 )
		{
			if ( opt )
				return false;
			opt.set_value( args[idx] );
			++idx;
		}
	}
	return true;
}

////////////////////////////////////////

bool
cmd_line::opt_many( option &opt, size_t &idx, const std::vector<std::string> &args )
{
	if ( !opt.is_non_option() )
		++idx;
	while ( idx < args.size() )
	{
		if ( args[idx].find( '-' ) == 0 )
		{
			if ( opt )
				break;
			throw_runtime( "option '{0}' needs at least one value", opt.name() );
		}
		opt.add_value( args[idx] );
		++idx;
	}
	return true;
}

////////////////////////////////////////

std::ostream &
operator<<( std::ostream &out, const cmd_line &cmdline )
{
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

		if ( !opt.args().empty() )
		{
			if ( tmp.empty() )
				tmp = opt.args();
			else
				tmp += " " + opt.args();
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

