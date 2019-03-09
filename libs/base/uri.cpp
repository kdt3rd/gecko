// Copyright (c) 2015-2016 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include "uri.h"
#include "string_util.h"
#include "contract.h"
#include <algorithm>
#include "file_system.h"
#include "user_info.h"

namespace base
{

////////////////////////////////////////

uri::uri( char *str )
{
	parse( str );
	while ( *str != '\0' )
	{
		*str = '*';
		++str;
	}
}

////////////////////////////////////////

std::string
uri::full_path( void ) const
{
	if ( _path.empty() )
		return std::string( 1, '/' );

	std::string ret;
	size_t n = 0;
	for ( auto &p: _path )
		n += p.size() + 1;
	ret.reserve( n );
	for ( auto &p: _path )
	{
		ret.push_back( '/' );
		ret.append( p );
	}
	return ret;
}

////////////////////////////////////////

std::vector<std::pair<std::string, std::string>>
uri::parse_query( char kv_sep, char arg_sep ) const
{
	std::vector<std::pair<std::string, std::string>> ret;
	if ( _raw_query.empty() )
		return ret;

	std::vector<std::string> args;
	base::split( _raw_query, arg_sep, std::back_inserter( args ), false );
	for ( auto &a: args )
	{
		std::string::size_type kvPos = a.find_first_of( kv_sep );
		if ( kvPos == std::string::npos )
			throw_runtime( "invalid key/value argument '{0}' - no separator '{2}'", a, kv_sep );

		std::string k = a.substr( 0, kvPos );
		std::string v = a.substr( kvPos + 1 );
		// NOW we unescape
		ret.emplace_back( std::make_pair( unescape( k ), unescape( v ) ) );
	}

	return ret;
}


////////////////////////////////////////

std::string uri::escape( cstring str )
{
	const cstring reserved = ":/?#[]@!$&'()*+,;=";
	std::string result;
	size_t i = 0;
	size_t n = str.find_first_of( reserved, i );
   	while ( n != cstring::npos )
	{
		result.append( str.data() + i, n - i );
		i = n;
		if ( i < str.size() )
		{
			result.append( base::format( "%{0,B16,w2}", int(str[i]) ) );
			++i;
		}
		n = str.find_first_of( reserved, i );
	}
	result.append( str.data() + i, std::min( str.size() - i, n ) );

	return result;
}

////////////////////////////////////////

std::string uri::unescape( cstring str )
{
	std::string result;
	result.reserve( str.size() );
	size_t i = 0;
	size_t n = str.find( '%', i );
   	while ( n != cstring::npos )
	{
		result.append( str.data() + i, n - i );
		i = n;
		if ( i < str.size() )
		{
			++i;
			if ( i+1 < str.size() )
			{
				if ( std::isxdigit( str[i] ) && std::isxdigit( str[i+1] ) )
					result.push_back( from_hex( str[i], str[i+1] ) );
				else
					throw_runtime( "invalid percent encoding at end: '{0}'", str );
			}
			else
				throw_runtime( "invalid percent encoding at end: '{0}'", str );
			i += 2;
		}
		n = str.find( '%', i );
	}
	result.append( str.data() + i, std::min( str.size() - i, n ) );

	return result;
}

////////////////////////////////////////

void uri::add_path( cstring path )
{
	split( path, '/', std::back_inserter( _path ), true );
}

////////////////////////////////////////

uri uri::root( void ) const
{
	uri result;
	result._scheme = _scheme;
	result._user = _user;
	result._host = _host;
	result._port = _port;
	return result;
}

////////////////////////////////////////

void uri::split_query( std::vector<std::pair<std::string,std::string>> &parsed )
{
	std::vector<std::string> list;
	split( list, _query, '&', true );
	for ( auto &q: list )
	{
		std::vector<std::string> fv;
		split( fv, q, '=' );
		if ( fv.size() != 2 )
			throw_runtime( "invalid field/value query '{0}'", q );
		parsed.emplace_back( std::move( fv[0] ), std::move( fv[1] ) );
	}
}

////////////////////////////////////////

std::string uri::pretty( void ) const
{
	std::stringstream out;
	out << scheme() << "://";
	if ( !host().empty() )
	{
		if ( !user().empty() )
			out << user() << '@';
		out << host();
		if ( port() != 0 )
			out << ':' << to_string( port() );
	}
	for ( auto &p: path() )
		out << '/' << p;
	if ( !query().empty() )
		out << '?' << query();
	if ( !fragment().empty() )
		out << '#' << fragment();
	return out.str();
}

////////////////////////////////////////

void uri::parse( cstring str )
{
	if ( str.empty() )
		return;

	cstring path;

	size_t colon = str.find( ':' );
	if ( colon == cstring::npos || str[0] == '/' || str[0] == '~' )
	{
		_scheme = "file";
		// assume it's a file path
		if ( str[0] == '~' )
		{
			size_t slpos = str.find( '/' );
			cstring user;
			if ( slpos != 1 )
				user = str.substr( 1, slpos );
			user_info uinf( user );
			add_path( uinf.home_dir() );
			if ( slpos != cstring::npos )
				path = str.substr( slpos + 1 );
		}
		else
		{
			if ( str[0] != '/' )
			{
				// get the current directory from the default filesystem
				uri tmp = file_system::get( _scheme )->current_path();
				_path = tmp._path;
			}

			path = str;
		}
	}
	else if ( str.size() > 2 && str[0] == '\\' && str[1] == '\\' )
	{
		// unc-like path
		throw_not_yet();
	}
	else
	{
		size_t question = str.find( '?', colon );
		size_t hash = str.find( '#', colon );

		if ( hash < question )
			question = cstring::npos;

		_scheme = unescape( str.substr( 0, colon ) );
		if ( colon < question )
			path = str.substr( colon + 1, std::min( hash, question ) - colon - 1 );
		if ( question < hash )
		{
			// stash the raw / unescaped to allow splitting
			_raw_query = str.substr( question + 1, hash - question - 1 );
			_query = unescape( _raw_query );
		}
		if ( hash != cstring::npos )
			_fragment = unescape( str.substr( hash + 1 ) );

		if ( path.empty() )
			throw_runtime( "invalid uri with no path: '{0}'", str );

		if ( begins_with( path, "//" ) )
		{
			size_t slash = path.find( '/', 2 );
			auto auth = path.substr( 2, slash - 2 );
			path.remove_prefix( slash );
			parse_authority( auth );
		}
		else if ( path[0] != '/' )
			throw_runtime( "expected uri path to start with slash: '{0}'", str );
	}

	add_path( path );
	for ( auto &p: _path )
		p = unescape( p );
}

////////////////////////////////////////

void uri::parse_authority( cstring auth )
{
	if ( auth.empty() )
		return;

	size_t at = auth.find( '@' );
	if ( at == cstring::npos )
		at = 0;
	else
		at = at + 1;
	size_t pcolon = auth.find( ':', at );

	if ( at > 0 && at < auth.size() )
		_user = unescape( auth.substr( 0, at - 1 ) );
	_host = unescape( auth.substr( at, pcolon - at ) );
	if ( pcolon < auth.size() )
		_port = static_cast<uint16_t>( stoul( auth.substr( pcolon + 1 ) ) );
	else
		_port = 0;
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

