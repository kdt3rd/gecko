
#include <iostream>
#include <map>
#include <utf/utf.h>
#include <base/contract.h>
#include "token.h"

namespace
{
	std::u32string newlines( U"\x000A\x000B\x000C\x000D\x0085\x2028\x2029" );

	const std::map<std::u32string,imgproc::token_type> keywords = {
		{ U"for", imgproc::TOK_FOR },
		{ U"public", imgproc::TOK_PUBLIC },
		{ U"function", imgproc::TOK_FUNCTION },
		{ U"if", imgproc::TOK_IF },
		{ U"else", imgproc::TOK_ELSE },
		{ U"to", imgproc::TOK_TO },
		{ U"by", imgproc::TOK_BY },
	};

	const std::set<char32_t> special =
	{
		U'{',
		U'}',
		U'(',
		U')',
		U',',
		U';',
		U':',
		U'=',
		U'\"',
		U'\'',
	};
}

////////////////////////////////////////

namespace imgproc
{

////////////////////////////////////////

iterator::iterator( std::istream &str, utf::mode m )
	: _utf( str, m ), _type( TOK_UNKNOWN )
{
	_c = *(++_utf);
}

////////////////////////////////////////

iterator &iterator::next( void )
{
	if ( !_next.empty() )
	{
		_value.swap( _next );
		_next.clear();
		return *this;
	}

	_type = TOK_UNKNOWN;
	_value.clear();

	// Skip any whitespace
	while ( utf::is_pattern_whitespace( _c ) && _utf )
		next_utf();

	_whitespace.swap( _value );
	_type = TOK_UNKNOWN;
	_value.clear();

	if ( !_utf )
		return *this;

	_start.set( _utf );

	if ( _c == '{' )
	{
		// Code block
		_type = TOK_BLOCK_START;
		next_utf();
	}
	else if ( _c == '}' )
	{
		// Code block
		_type = TOK_BLOCK_END;
		next_utf();
	}
	else if ( _c == '(' )
	{
		// Expression ordering
		_type = TOK_PAREN_START;
		next_utf();
	}
	else if ( _c == ')' )
	{
		// Expression ordering
		_type = TOK_PAREN_END;
		next_utf();
	}
	else if ( _c == ',' )
	{
		// Argument separator
		_type = TOK_COMMA;
		next_utf();
	}
	else if ( _c == ';' )
	{
		// Statement terminator
		_type = TOK_EXPRESSION_END;
		next_utf();
	}
	else if ( _c == ':' )
	{
		// Separator
		_type = TOK_SEPARATOR;
		next_utf();
	}
	else if ( _c == '=' )
	{
		// Assignment separator
		_type = TOK_ASSIGN;
		next_utf();
	}
	else if ( _c == '\"' )
		parse_string();
	else if ( _c == '\'' )
		parse_char();
	else if ( _c == 0x2AFD || _c == '#' ) // Double slash character or hash
	{
		_whitespace.push_back( _c );
		parse_comment();
	}
	else if ( _c == '/' || _c == 0x29F8 || _c == 0xFF0F || _c == 0x2044 ) // Slash, big slash, and fullwidth slash, fraction slash
	{
		char32_t comment_start = _c;
		skip_utf();
		if ( _c == comment_start || _c == U'*' )
			parse_comment();
		else
		{
			_value += comment_start;
			parse_operator();
		}
	}
	else if ( utf::is_identifier_start( _c ) )
		parse_identifier();
	else if ( utf::is_pattern_syntax( _c ) )
		parse_operator();
	else if ( utf::is_number_decimal( _c ) )
		parse_number();
	else
		next_utf();

	utf::nfc( _whitespace );
	utf::nfc( _value );

	_end.set( _utf );

	return *this;
}

////////////////////////////////////////

bool iterator::split( const std::u32string &s )
{
	precondition( _next.empty(), "cannot split multiple times" );
	precondition( !s.empty(), "cannot split nothing" );

	if ( _value.compare( 0, s.size(), s ) == 0 )
	{
		_next = _value.substr( s.size() );
		_value.resize( s.size() );
		return true;
	}

	return false;
}

////////////////////////////////////////

bool iterator::split( const char32_t *s )
{
	size_t n = std::char_traits<char32_t>::length( s );
	if ( _value.compare( 0, n, s ) == 0 )
	{
		_next = _value.substr( n );
		_value.resize( n );
		return true;
	}

	return false;
}

////////////////////////////////////////

bool iterator::split( size_t n )
{
	precondition( _next.empty(), "cannot split multiple times" );
	if ( n >= _value.size() )
		return false;

	_next = _value.substr( n );
	_value.resize( n );
	return true;
}

////////////////////////////////////////

void iterator::skip_comments( void )
{
	while ( _type == TOK_COMMENT )
		this->next();
}

////////////////////////////////////////

void iterator::parse_operator( void )
{
	_type = TOK_OPERATOR;
	while ( utf::is_pattern_syntax( _c ) && special.find( _c ) == special.end() )
		next_utf();
}

////////////////////////////////////////

void iterator::parse_identifier( void )
{
	_type = TOK_IDENTIFIER;
	while ( _utf && utf::is_identifier_continue( _c ) )
		next_utf();
	auto k = keywords.find( _value );
	if ( k != keywords.end() )
		_type = k->second;
}

////////////////////////////////////////

void iterator::parse_comment( void )
{
	if ( _c == U'*' )
	{
		skip_utf();
		bool star = false;
		while ( true )
		{
			if ( star )
			{
				if ( _c == U'/' )
				{
					skip_utf();
					break;
				}
				else
					_value.push_back( U'*' );
			}

			if ( _c == U'*' )
			{
				star = true;
				skip_utf();
			}
			else
			{
				star = false;
				next_utf();
			}
		}
		_type = TOK_COMMENT;
	}
	else
	{
		skip_utf();

		while( newlines.find( _c ) == std::u32string::npos )
		{
			next_utf();
			if ( !_utf )
				break;
		}

		_type = TOK_COMMENT;
	}
}

////////////////////////////////////////

void iterator::parse_string( void )
{
	if ( _c != '\"' )
		return;
	
	skip_utf();

	while( _c != '\"' )
	{
		if ( _c == '\\' )
			parse_escape();
		next_utf();
		if ( ! _utf )
			return;
	}
	skip_utf();
	_type = TOK_STRING;
}

////////////////////////////////////////

void iterator::parse_char( void )
{
	if ( _c != '\'' )
		return;
	
	skip_utf();

	while( _c != '\'' )
	{
		if ( _c == '\\' )
			parse_escape();
		next_utf();
		if ( ! _utf )
			return;
	}
	skip_utf();
	_type = TOK_CHARACTER;
}

////////////////////////////////////////

void iterator::parse_number( void )
{
	// Parse a decimal number
	int64_t v = utf::integer_value( _c );
	
	if ( v == 0 )
	{
		next_utf();
		// Number with different bases
		switch ( _c )
		{
			case 'b':
			case 'B':
				// Binary number
				next_utf();
				while ( utf::is_number_decimal( _c ) )
				{
					v = utf::integer_value( _c );
					if ( v < 0 || v > 1 )
						break;;
					next_utf();
				}
				_type = TOK_NUMBER;
				break;

			case 'c':
			case 'C':
				// Octal number
				next_utf();
				while ( utf::is_number_decimal( _c ) )
				{
					v = utf::integer_value( _c );
					if ( v < 0 || v > 7 )
						break;
					next_utf();
				}
				_type = TOK_NUMBER;
				break;

			case 'x':
			case 'X':
				// Hexadecimal number
				next_utf();
				while ( utf::is_hex_digit( _c ) )
					next_utf();
				_type = TOK_NUMBER;
				break;

			default:
				// Unknown base...
				if ( utf::is_number( _c ) )
				{
					// Numbers shouldn't start with 0 without a base...
					while ( utf::is_number( *_utf ) )
						next_utf();
					_type = TOK_UNKNOWN;
				}
				else
				{
					// else it's just a 0 by itself, which is okay... continue as a decimal
					parse_decimal();
				}
				break;
		}
	}
	else if ( v > 0 )
		parse_decimal();
}

////////////////////////////////////////

void iterator::parse_decimal( void )
{
	while ( utf::is_number_decimal( _c ) )
		next_utf();

	if ( _c == '.' )
	{
		next_utf();
		while ( utf::is_number_decimal( _c ) )
			next_utf();
	}

	if ( _c == 'e' || _c == 'E' )
	{
		next_utf();
		if ( _c == '-' || _c == '+' )
			next_utf();
		while ( utf::is_number_decimal( _c ) )
			next_utf();
	}
	_type = TOK_NUMBER;
}

////////////////////////////////////////

void iterator::parse_escape( void )
{
	// Special processing of escape sequences
	++_utf;
	_c = *_utf;
	int64_t c = 0;
	switch ( _c )
	{
		case 'a': c = '\a'; break;
		case 'n': c = '\n'; break;
		case 'r': c = '\r'; break;
		case 't': c = '\t'; break;
		case 'b': c = '\b'; break;
		case 'f': c = '\f'; break;
		case 'v': c = '\v'; break;
		case '\"': c = '\"'; break;
		case '\'': c = '\''; break;
		case '\\': c = '\\'; break;
		case '0': c = 0; break;
		case 'x':
			for ( int i = 0; i < 2; ++i )
			{
				_c = *(++_utf);
				if ( !utf::is_hex_digit( _c ) )
					throw std::runtime_error( "Not a hex digit" );
				int64_t v = utf::integer_value( _c );
				c = c * 16 + v;
			}
			break;

		case 'u':
			for ( int i = 0; i < 4; ++i )
			{
				_c = *(++_utf);
				if ( !utf::is_hex_digit( _c ) )
					throw std::runtime_error( "Not a hex digit" );
				int64_t v = utf::integer_value( _c );
				c = c * 16 + v;
			}
			break;

		case 'U':
			for ( int i = 0; i < 8; ++i )
			{
				_c = *(++_utf);
				if ( !utf::is_hex_digit( _c ) )
					throw std::runtime_error( "Not a hex digit" );
				int64_t v = utf::integer_value( _c );
				c = c * 16 + v;
			}
			break;

		case 'c':
			for ( int i = 0; i < 3; ++i )
			{
				_c = *(++_utf);
				if ( !utf::is_hex_digit( _c ) )
					throw std::runtime_error( "Not an octal digit" );
				int64_t v = utf::integer_value( _c );
				if ( v > 7 )
					throw std::runtime_error( "Not an octal digit" );
				c = c * 8 + v;
			}
			break;

		case 'd':
			for ( int i = 0; i < 3; ++i )
			{
				_c = *(++_utf);
				if ( !utf::is_decimal( _c ) )
					throw std::runtime_error( "Not a decimal digit" );
				int64_t v = utf::integer_value( _c );
				c = c * 10 + v;
			}
			break;

		default:
			throw std::runtime_error( "Invalid escape sequence" );
	}

	_c = static_cast<char32_t>( c );
}

////////////////////////////////////////

void iterator::next_utf( void )
{
	_value += _c;
	++_utf;
	_c = *_utf;
}

////////////////////////////////////////

void iterator::skip_utf( void )
{
	++_utf;
	_c = *_utf;
}

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const iterator &i )
{
	out << i.type() << ' ' << i.value();
	return out;
}

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, token_type t )
{
	out << token_name( t );
	return out;
}

////////////////////////////////////////

const char *token_name( token_type t )
{
	switch ( t )
	{
		case TOK_ASSIGN:         return "assign";
		case TOK_BLOCK_END:      return "block end";
		case TOK_BLOCK_START:    return "block start";
		case TOK_CHARACTER:      return "character";
		case TOK_COMMA:          return "comma";
		case TOK_COMMENT:        return "comment";
		case TOK_IDENTIFIER:     return "identifier";
		case TOK_NUMBER:         return "number";
		case TOK_PAREN_END:      return "paren end";
		case TOK_PAREN_START:    return "paren open";
		case TOK_EXPRESSION_END: return "expr end";
		case TOK_SEPARATOR:      return "separator";
		case TOK_STRING:         return "string";
		case TOK_OPERATOR:       return "operator";
		case TOK_IF:
		case TOK_ELSE:
		case TOK_FUNCTION:
		case TOK_TO:
		case TOK_BY:
		case TOK_FOR:            return "keyword";
		default:		         return "unknown";
	}
}

////////////////////////////////////////

}

