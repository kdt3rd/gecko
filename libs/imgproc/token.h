
#pragma once

#include <iostream>
#include <string>
#include <set>

#include <utf/utf.h>
#include "location.h"

namespace imgproc
{

////////////////////////////////////////

enum token_type
{
	TOK_BLOCK_END,
	TOK_BLOCK_START,
	TOK_CHARACTER,
	TOK_COMMA,
	TOK_COMMENT,
	TOK_IDENTIFIER,
	TOK_NUMBER,
	TOK_PAREN_END,
	TOK_PAREN_START,
	TOK_SEPARATOR,
	TOK_EXPRESSION_END,
	TOK_STRING,
	TOK_OPERATOR,
	TOK_ASSIGN,
	TOK_IF,
	TOK_FOR,
	TOK_PUBLIC,
	TOK_FUNCTION,
	TOK_ELSE,
	TOK_TO,
	TOK_BY,
	TOK_UNKNOWN,
};

////////////////////////////////////////

class iterator
{
public:
	iterator( std::istream &str, utf::mode m = utf::UTF8 );

	token_type type( void ) const { return _type; }

	size_t size( void ) const { return _value.size(); }
	const std::u32string &value( void ) const { return _value; }
	std::u32string &value( void ) { return _value; }
	const std::u32string &whitespace( void ) const { return _whitespace; }

	iterator &next( void );
	bool split( const std::u32string &s );
	bool split( const char32_t *s );
	bool split( size_t n );

	inline operator bool() const { return (bool)_utf; }

	void skip_comments( void );

	inline const location &start_location( void ) const { return _start; }
	inline const location &end_location( void ) const { return _end; }

private:
	void parse_operator( void );
	void parse_identifier( void );
	void parse_comment( void );
	void parse_string( void );
	void parse_char( void );
	void parse_number( void );
	void parse_decimal( void );
	void parse_escape( void );

	void next_utf( void );
	void skip_utf( void );

	location _start;
	location _end;
	utf::iterator _utf;
	char32_t _c = 0;
	token_type _type;
	std::u32string _whitespace;
	std::u32string _value;
	std::u32string _next;
};

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const iterator &i );
std::ostream &operator<<( std::ostream &out, token_type t );
const char *token_name( token_type t );

////////////////////////////////////////

}

