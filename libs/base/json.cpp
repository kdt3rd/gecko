//
// Copyright (c) 2015-2016 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "json.h"
#include <iterator>

namespace
{
const base::json &
error_obj( void )
{
	static base::json error;
	return error;
}

}

namespace base
{

////////////////////////////////////////

json::~json( void )
{
	clear();
}

////////////////////////////////////////

void json::parse( const std::string &str )
{
	std::stringstream tmp( str );
	parse( tmp );
}

////////////////////////////////////////

void json::parse( std::istream &in )
{
	auto flags = in.flags();
	in.unsetf( std::ios_base::skipws );

	std::istream_iterator<char> it( in );
	std::istream_iterator<char> end;

	int line = 1;
	parse_value( it, end, line );

	in.flags( flags );
}

////////////////////////////////////////

size_t json::size( void ) const
{
	if ( is<json_object>() )
		return get<json_object>().size();
	if ( is<json_array>() )
		return get<json_array>().size();
	if ( is<json_string>() )
		return get<json_string>().size();
	return 0;
}

////////////////////////////////////////

const json &json::operator[]( const std::string &name ) const
{
	precondition( is<json_object>(), "not a json object" );
	const json_object &o = get<json_object>();
	auto x = o.find( name );
	if ( x == o.end() )
		return error_obj();
	return x->second;
}

////////////////////////////////////////

json &json::operator[]( const std::string &name )
{
	if ( !valid() || is<json_null>() )
		set<json_object>();

	precondition( is<json_object>(), "not a json object" );
	return get<json_object>()[name];
}

////////////////////////////////////////

const json &json::operator[]( size_t idx ) const
{
	precondition( is<json_array>(), "not a json array" );
	const json_array &a = get<json_array>();
	if ( idx < a.size() )
		return a[idx];
	return error_obj();
}

////////////////////////////////////////

json &json::operator[]( size_t idx )
{
	precondition( is<json_array>(), "not a json array" );
	json_array &a = get<json_array>();
	if ( idx < a.size() )
		a.resize( idx + 1 );
	return a[idx];
}

////////////////////////////////////////

const json &json::at( const std::string &name ) const
{
	precondition( is<json_object>(), "not a json object" );
	const auto &o = get<json_object>();
	auto x = o.find( name );
	if ( x == o.end() )
		return error_obj();
	return x->second;
}

////////////////////////////////////////

json &json::at( const std::string &name )
{
	if ( !valid() || is<json_null>() )
		set<json_object>();

	precondition( is<json_object>(), "not a json object" );
	return get<json_object>().at( name );
}

////////////////////////////////////////

const json &json::at( size_t idx ) const
{
	precondition( is<json_array>(), "not a json array" );
	const auto &a = get<json_array>();
	if ( idx < a.size() )
		return a.at( idx );
	return error_obj();
}

////////////////////////////////////////

json &json::at( size_t idx )
{
	precondition( is<json_array>(), "not a json array" );
	json_array &a = get<json_array>();
	if ( idx < a.size() )
		a.resize( idx + 1 );
	return a.at( idx );
}

////////////////////////////////////////

bool json::has( const std::string &name ) const
{
	precondition( is<json_object>(), "not a json object" );
	auto &obj = get<json_object>();
	return obj.find( name ) != obj.end();
}

////////////////////////////////////////

void json::push_back( const json &x )
{
	if ( !valid() || is<json_null>() )
		set<json_array>();

	precondition( is<json_array>(), "not a json array" );
	get<json_array>().push_back( x );
}

////////////////////////////////////////

void json::push_back( json &&x )
{
	if ( !valid() || is<json_null>() )
		set<json_array>();

	precondition( is<json_array>(), "not a json array" );
	get<json_array>().push_back( std::move( x ) );
}

////////////////////////////////////////

json &json::push_back( void )
{
	if ( !valid() || is<json_null>() )
		set<json_array>();

	precondition( is<json_array>(), "not a json array" );
	auto &tmp = get<json_array>();
	tmp.push_back( json_null() );
	return tmp.back();
}

////////////////////////////////////////

void json::append( json &&that )
{
	precondition( is<json_object>(), "receiving object not a json object" );
	precondition( that.is<json_object>(), "source not a json object" );
	json_object &m = get<json_object>();
	json_object &o = that.get<json_object>();
	for ( auto &i: o )
		m[i.first] = i.second;
}

////////////////////////////////////////

void json::dump( void ) const
{
	std::cout << *this << std::endl;
}

////////////////////////////////////////

void json::parse_value( std::istream_iterator<char> &it, std::istream_iterator<char> &end, int &line )
{
	skip_whitespace( it, end, line );

	if ( it == end )
	{
		clear();
		return;
	}

	switch ( *it )
	{
		case '[':
			parse_array( it, end, line );
			break;

		case '{':
			parse_object( it, end, line );
			break;

		case '"':
			parse_string( it, end, line );
			break;

		case 't':
			parse_true( it, end, line );
			break;

		case 'f':
			parse_false( it, end, line );
			break;

		case 'n':
			parse_null( it, end, line );
			break;

		default:
			parse_number( it, end, line );
	}
}

////////////////////////////////////////

void json::parse_array( std::istream_iterator<char> &it, std::istream_iterator<char> &end, int &line )
{
	clear();

	skip_whitespace( it, end, line );
	if ( *it != '[' )
		throw_runtime( "invalid json value, expecting array open, at line {0}", line );
	++it;

	json_array arr;
	json val;
	while ( it != end )
	{
		skip_whitespace( it, end, line );
		if ( *it == ']' )
			break;

		val.parse_value( it, end, line );
		arr.push_back( val );

		skip_whitespace( it, end, line );
		if ( *it == ']' )
			break;

		if ( *it != ',' )
			throw_runtime( "expected ',' or ']' in json array at line {0}", line );
		++it;
	}

	if ( *it != ']' )
		throw_runtime( "unterminated json array at line {0}", line );
	++it;

	set<json_array>( std::move( arr ) );
}

////////////////////////////////////////

void json::parse_object( std::istream_iterator<char> &it, std::istream_iterator<char> &end, int &line )
{
	clear();

	skip_whitespace( it, end, line );
	if ( *it != '{' )
		throw_runtime( "invalid json value, expecting object open, at line {0}", line );
	++it;

	json_object obj;
	json name;
	json val;
	skip_whitespace( it, end, line );
	if ( *it != '}' )
	{
		while ( it != end )
		{
			name.parse_string( it, end, line );
			skip_whitespace( it, end, line );
			if ( *it != ':' )
				throw_runtime( "expected ':' in json object at line {0} (got {1})", line, *it );
			++it;
			skip_whitespace( it, end, line );
			val.parse_value( it, end, line );

			obj[name.get<std::string>()] = std::move( val );

			skip_whitespace( it, end, line );
			if ( *it == '}' )
				break;

			if ( *it != ',' )
				throw_runtime( "expected ',' or '}' in json object at line {0} (got {1})", line, *it );
			++it;
			skip_whitespace( it, end, line );
		}
	}

	if ( *it != '}' )
		throw_runtime( "unterminated json object at line {0}", line );
	++it;

	set<json_object>( std::move( obj ) );
}

////////////////////////////////////////

void json::parse_string( std::istream_iterator<char> &it, std::istream_iterator<char> &end, int &line )
{
	clear();

	std::string n;

	if ( *it != '"' )
		throw_runtime( "invalid json value, expecting string open but got {1}, at line {0}", line, *it );
	++it;

	while ( it != end )
	{
		char c = *it;
		if ( c == '"' )
			break;
		if ( c == '\\' )
		{
			++it;
			if ( it == end )
				throw_runtime( "unterminated json string at line {0}", line );

			c = *it;
			switch ( c )
			{
				case 'b': n.push_back( '\b' ); break;
				case 'f': n.push_back( '\f' ); break;
				case 'n': n.push_back( '\n' ); break;
				case 't': n.push_back( '\t' ); break;
				case 'r': n.push_back( '\r' ); break;
				case 'u':
				{
					uint32_t u = 0;
					for ( size_t i = 0; i < 4; ++i )
					{
						++it;
						if ( it == end )
							throw_runtime( "unterminated unicode at end of string" );
						c = static_cast<char>( std::toupper( *it ) );
						if ( !std::isxdigit( c ) )
							throw_runtime( "expected hex digit, got {0}", c );
						u = ( u << 4 ) + uint32_t( (c > '9') ? (c & ~0x20) - 'A' + 10: ( c - '0' ) );
					}
					n.push_back( static_cast<char>( u ) );
					break;
				}

				default:
					n.push_back( c );
					break;
			}
		}
		else
			n.push_back( c );
		++it;
	}

	if ( *it != '"' )
		throw_runtime( "unterminated json string at line {0}", line );
	++it;

	set<json_string>( n );
}

////////////////////////////////////////

void json::parse_true( std::istream_iterator<char> &it, std::istream_iterator<char> &end, int &line )
{
	clear();

	const char *next = "true";
	while ( it != end && *next != '\0' )
	{
		if ( *it != *next )
			throw_runtime( "invalid json value validating boolean true at line {0}", line );
		++next;
		++it;
	}

	if ( *next != '\0' )
		throw_runtime( "invalid json, extra chars validating boolean true value, at line {0}", line );

	set<bool>( true );
}

////////////////////////////////////////

void json::parse_false( std::istream_iterator<char> &it, std::istream_iterator<char> &end, int &line )
{
	clear();

	const char *next = "false";
	while ( it != end && *next != '\0' )
	{
		if ( *it != *next )
			throw_runtime( "invalid json value validating boolean false at line {0}", line );
		++next;
		++it;
	}

	if ( *next != '\0' )
		throw_runtime( "invalid json value, extra values validating boolean false, at line {0}", line );

	set<bool>( false );
}

////////////////////////////////////////

void json::parse_null( std::istream_iterator<char> &it, std::istream_iterator<char> &end, int &line )
{
	clear();

	const char *next = "null";
	while ( it != end && *next != '\0' )
	{
		if ( *it != *next )
			throw_runtime( "invalid json value validating null at line {0}", line );
		++next;
		++it;
	}

	if ( *next != '\0' )
		throw_runtime( "invalid json value, extra values validating null, at line {0}", line );

	set<json_null>();
}

////////////////////////////////////////

void json::parse_number( std::istream_iterator<char> &it, std::istream_iterator<char> &end, int &line )
{
	clear();
	std::string n;
	while ( it != end )
	{
		char c = *it;
		if ( std::isdigit( c ) || c == '.' || c == 'e' || c == 'E' || c == '-' || c == '+' )
			n.push_back( c );
		else
			break;
		++it;
	}

	if ( n.empty() )
		throw_runtime( "invalid json value parsing number at line {0} (character {1})", line, *it );

	size_t pos = 0;
	double x = std::stod( n, &pos );
	if ( pos < n.size() )
		throw_runtime( "invalid json number \"{0}\" at line {1}", n, line );

	set<json_number>( std::make_pair( x, n ) );
}

////////////////////////////////////////

void json::skip_whitespace( std::istream_iterator<char> &it, std::istream_iterator<char> &end, int &line )
{
	while ( it != end )
	{
		if ( *it == '\n' )
			line++;
		else if ( !std::isspace( *it ) )
			return;
		++it;
	}
}

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const json &j )
{
	precondition( j.valid(), "invalid json value" );

	if ( j.is<json_number>() )
		out << j.get<json_number>().second;
	else if ( j.is<json_string>() )
	{
		out << '\"';
		for ( auto c: j.get<json_string>() )
		{
			switch ( c )
			{
				case '"':
				case '\\':
				case '/':
					out << '\\' << c;
					break;

				case '\b': out << '\\' << 'b'; break;
				case '\f': out << '\\' << 'f'; break;
				case '\r': out << '\\' << 'r'; break;
				case '\n': out << '\\' << 'n'; break;
				case '\t': out << '\\' << 't'; break;

				default:
					if ( std::iscntrl( c ) )
						out << format( "\\u{0,w4,b16,f0,ar}", static_cast<int>(c) );
					else
						out << c;
					break;
			}
		}
		out << '\"';
	}
	else if ( j.is<json_bool>() )
	{
		if ( j.get<json_bool>() )
			out << "true";
		else
			out << "false";
	}
	else if ( j.is<json_null>() )
		out << "null";
	else if ( j.is<json_array>() )
	{
		const auto &arr = j.get<json_array>();
		bool first = true;
		out << '[';
		for ( const auto &i: arr )
		{
			if ( !first )
				out << ',';
			out << i;
			first = false;
		}
		out << ']';
	}
	else if ( j.is<json_object>() )
	{
		const auto &obj = j.get<json_object>();
		bool first = true;
		out << '{';
		for ( const auto &i: obj )
		{
			if ( !first )
				out << ',';
			out << '"' << i.first << "\":" << i.second;
			first = false;
		}
		out << '}';
	}

	return out;
}

////////////////////////////////////////

void operator<<( std::string &str, const json &j )
{
	std::stringstream out;
	out << j;
	str = out.str();
}

////////////////////////////////////////

}

