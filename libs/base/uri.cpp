
#include "uri.h"
#include "string_util.h"
#include "contract.h"
#include <algorithm>

namespace base
{

////////////////////////////////////////

uri::uri( const std::string &str )
	: _path()
{
	parse( str );
}

////////////////////////////////////////

std::string uri::escape( const std::string &str )
{
	const std::string reserved = ":/?#[]@!$&'()*+,;=";
	std::string result;
	size_t i = 0;
	size_t n = str.find_first_of( reserved, i );
   	while ( n < str.size() )
	{
		result.append( str, i, n - i );
		i = n;
		if ( i < str.size() )
		{
			result.append( base::format( "%{0,B16,w2}", int(str[i]) ) );
			++i;
		}
		n = str.find_first_of( reserved, i );
	}
	result.append( str, i, n );

	return result;
}

////////////////////////////////////////

std::string uri::unescape( const std::string &str )
{
	std::string result;
	result.reserve( str.size() );
	size_t i = 0;
	size_t n = str.find( '%', i );
   	while ( n < str.size() )
	{
		result.append( str, i, n - i );
		i = n;
		if ( i < str.size() )
		{
			++i;
			if ( i+1 < str.size() )
			{
				if ( isxdigit( str[i] ) && isxdigit( str[i+1] ) )
					result.push_back( static_cast<char>( std::stoi( str.substr( i, 2 ), nullptr, 16 ) ) );
				else
					throw_runtime( "invalid percent encoding at end: '{0}'", str );
			}
			else
				throw_runtime( "invalid percent encoding at end: '{0}'", str );
			i += 2;
		}
		n = str.find( '%', i );
	}
	result.append( str, i, n );

	return result;
}

////////////////////////////////////////

void uri::split_query( std::vector<std::pair<std::string,std::string>> &parsed )
{
	std::vector<std::string> list;
	split( _query, '&', std::back_inserter( list ) );
	for ( auto &q: list )
	{
		std::vector<std::string> fv;
		split( q, '=', std::back_inserter( fv ) );
		if ( fv.size() != 2 )
			throw_runtime( "invalid field/value query '{0}'", q );
		parsed.emplace_back( std::move( fv[0] ), std::move( fv[1] ) );
	}
}

////////////////////////////////////////

std::shared_ptr<std::istream> uri::open_for_read( void )
{
	auto &o = _readers.at( _scheme );
	return o( *this );
}

////////////////////////////////////////

std::shared_ptr<std::ostream> uri::open_for_write( void )
{
	auto &o = _writers.at( _scheme );
	return o( *this );
}

////////////////////////////////////////

std::map<std::string,std::function<std::shared_ptr<std::istream>(const base::uri &)>> uri::_readers;
std::map<std::string,std::function<std::shared_ptr<std::ostream>(const base::uri &)>> uri::_writers;

////////////////////////////////////////

void uri::parse( const std::string &str )
{
	if ( str.empty() )
		return;

	std::string path;

	if ( str[0] != '/' )
	{
		size_t colon = str.find( ':' );
		if ( colon >= str.size() )
			throw_runtime( "invalid uri missing colon: '{0}'", str );
		size_t question = str.find( '?', colon );
		size_t hash = str.find( '#', colon );

		if ( hash < question )
			question = std::string::npos;

		_scheme = unescape( str.substr( 0, colon ) );
		if ( colon < question )
			path = str.substr( colon + 1, std::min( hash, question ) - colon - 1 );
		if ( question < hash )
			_query = unescape( str.substr( question + 1, hash - question - 1 ) );
		if ( hash  < str.size() )
			_fragment = unescape( str.substr( hash + 1 ) );

		if ( path.empty() )
			throw_runtime( "invalid uri with no path: '{0}'", str );

		if ( begins_with( path, "//" ) )
		{
			size_t slash = path.find( '/', 2 );
			auto auth = path.substr( 2, slash - 2 );
			path.erase( 0, slash );

			size_t at = auth.find( '@' );
			if ( at > auth.size() )
				at = 0;
			else
				at = at + 1;
			size_t pcolon = auth.find( ':', at );

			if ( at > 0 && at < auth.size() )
				_user = unescape( auth.substr( 0, at - 1 ) );
			_host = unescape( auth.substr( at, pcolon - at ) );
			if ( pcolon < auth.size() )
				_port = std::stoi( auth.substr( pcolon + 1 ) );
			else
				_port = 0;
		}
		else if ( path[0] != '/' )
			throw_runtime( "expected uri path to start with slash: '{0}'", str );

	}
	else
		path = str;

	split( path, '/', std::back_inserter( _path ) );
	_path.erase( std::remove( _path.begin(), _path.end(), std::string() ), _path.end() );
	for ( auto &p: _path )
		p = unescape( p );
}

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const uri &u )
{
	out << uri::escape( u.scheme() ) << ':';
	if ( !u.host().empty() )
	{
		out << "//";
		if ( !u.user().empty() )
			out << uri::escape( u.user() ) << '@';
		out << uri::escape( u.host() );
		if ( u.port() != 0 )
			out << ':' << to_string( u.port() );
	}
	for ( auto &p: u.path() )
		out << '/' << uri::escape( p );
	if ( !u.query().empty() )
		out << '?' << uri::escape( u.query() );
	if ( !u.fragment().empty() )
		out << '#' << uri::escape( u.fragment() );
	return out;
}

////////////////////////////////////////

}

