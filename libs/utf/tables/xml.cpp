
#include <stdexcept>
#include <vector>
#include <stdlib.h>

#include "xml.h"

namespace {

////////////////////////////////////////

// Eat whitespace
void skip_space( std::istreambuf_iterator<char> &i, std::istreambuf_iterator<char> &eos )
{
	while ( i != eos && isspace( *i ) )
		++i;
}

////////////////////////////////////////

void find_tag( std::istreambuf_iterator<char> &i, std::istreambuf_iterator<char> &eos )
{
	skip_space( i, eos );

	// We better have a '<'
	if ( i != eos )
	{
		if ( *i != '<' )
		{
			std::cerr << "GOT " << (char)*i << std::endl;
			throw std::runtime_error( "XML parse error (expected <)" );
		}
		++i;
	}
}

////////////////////////////////////////

void
read_name( std::istreambuf_iterator<char> &i, std::istreambuf_iterator<char> &eos, std::string &name )
{
	name.clear();

	skip_space( i, eos );

	if ( i != eos && ( isalpha( *i ) || *i == ':' || *i == '_' ) )
	{
		while ( i != eos && ( isalnum( *i ) || *i == ':' || *i == '_' || *i == '-' || *i == '.' ) )
		{
			name.push_back( *i );
			++i;
		}
	}

	skip_space( i, eos );
	if ( name.empty() )
	{
		std::cerr << "Got " << (char)*i << std::endl;
		throw std::runtime_error( "Expected XML name" );
	}
}

////////////////////////////////////////

void
decode( std::string &val )
{
	for ( size_t i = 0; i+2 < val.size(); ++i )
	{
		if ( val[i] == '&' && val[i + 1] == '#' && val[i + 4] == ';' )
		{
			char hex[3] = "XX";
			hex[0] = val[i+2];
			hex[1] = val[i+3];

			char ch[2] = "";
			ch[0] = (char)strtol( hex, NULL, 16 );

			val.replace( i, 5, ch );
			--i;
		}
	}
}

////////////////////////////////////////

void
read_value( std::istreambuf_iterator<char> &i, std::istreambuf_iterator<char> &eos, std::string &value )
{
	value.clear();

	skip_space( i, eos );

	if ( i == eos || *i != '\"' )
		throw std::runtime_error( "Attribute value expected '\"'" );
	++i;

	while ( i != eos && *i != '\"' )
	{
		value.push_back( *i );
		++i;
	}

	if ( i == eos )
		throw std::runtime_error( "Attribute value expected ending '\"'" );

	++i;
	skip_space( i, eos );
	decode( value );
}

////////////////////////////////////////

void
read_content( std::istreambuf_iterator<char> &i, std::istreambuf_iterator<char> &eos, std::string &content )
{
	content.clear();

	// Read until a tag start
	while( *i != '<' )
	{
		if ( i == eos )
			throw std::runtime_error( "XML parse error (end of stream) reading content" );

		content.push_back( *i );
		++i;
	}
	decode( content );
}

////////////////////////////////////////

}

namespace xml
{

////////////////////////////////////////

reader::~reader( void )
{
}

////////////////////////////////////////

void
reader::parse( std::istream &stream )
{
	try{
	std::istreambuf_iterator<char> i( stream );
	std::istreambuf_iterator<char> eos;

	std::string start;

	std::vector<std::string> stack;
	std::string name, value;

	find_tag( i, eos );
	while ( i != eos )
	{
		if ( *i == '/' )
		{
			// End tags.
			++i;
			read_name( i, eos, name );
			if ( name != stack.back() )
				throw std::runtime_error( "mismatched XML tags" );
			stack.pop_back();
			if ( i == eos || *i != '>' )
				throw std::runtime_error( "expected closing '>' to end tag" );
			++i;
			find_tag( i, eos );
			continue;
		}
		else if ( *i == '!' )
		{
			// Deal with comment
			++i;
			if ( i == eos || *i != '-' )
				throw std::runtime_error( "expected comment" );
			++i;
			if ( i == eos || *i != '-' )
				throw std::runtime_error( "expected comment" );
			++i;

end_of_comment:
			while ( i != eos && *i != '-' )
				++i;
			if ( i == eos )
				throw std::runtime_error( "expected comment end" );
			++i;
			if ( i == eos )
				throw std::runtime_error( "expected comment end" );
			if ( *i != '-' )
				goto end_of_comment;
			++i;
			if ( i == eos )
				throw std::runtime_error( "expected comment end" );
			if ( *i != '>' )
				goto end_of_comment;
			++i;
			find_tag( i, eos );
			continue;
		}
		else if ( *i == '?' )
		{
			++i;

end_of_special:
			while ( i != eos && *i != '?' )
				++i;
			if ( i == eos )
				throw std::runtime_error( "expected comment end" );
			++i;
			if ( *i != '>' )
				goto end_of_special;
			++i;
			find_tag( i, eos );
			continue;
		}

		read_name( i, eos, name );
		stack.push_back( name );

		begin_element( stack.back() );

		bool skip_content = false;

		while ( i != eos && *i != '>' )
		{
			if ( *i == '/' )
			{
				end_element( stack.back() );
				stack.pop_back();
				skip_space( i, eos );
				++i;
				if ( i == eos || *i != '>' )
					throw std::runtime_error( "expected closing '>' to end tag" );
				skip_content = true;
				break;
			}

			read_name( i, eos, name );
			if ( i == eos || *i != '=' )
				throw std::runtime_error( "Expected '=' for attribute value" );
			++i;
			read_value( i, eos, value );
			attribute( name, value );
		}
		++i;

		if ( !skip_content )
		{
			read_content( i, eos, value );
			content( value );
		}
		find_tag( i, eos );
	}
	}
	catch ( ... )
	{
		std::cerr << "Stream position: " << stream.tellg() << std::endl;
		throw;
	}
}

////////////////////////////////////////

/*
std::string
encode( const std::string &val )
{
	std::string str( val );

	for ( size_t i = 0; i != str.size(); ++i )
	{
		if ( str[i] <= 32 || (unsigned char)(str[i]) >= 128 )
		{
			char tmp[6];
			sprintf( tmp, "&#%2.2X;", str[i] );
			str.replace( i, 1, tmp );
			i += 4;
		}
		else
		{
			switch( str[i] )
			{
				case '&':
					str.replace( i, 1, "&#26;" );
					i += 4;
					break;
				case ';':
					str.replace( i, 1, "&#3B;" );
					i += 4;
					break;
				case '<':
					str.replace( i, 1, "&#3C;" );
					i += 4;
					break;
				case '>':
					str.replace( i, 1, "&#3E;" );
					i += 4;
					break;
				default:
					break;
			}
		}
	}

	return str;
}
*/

////////////////////////////////////////

}

