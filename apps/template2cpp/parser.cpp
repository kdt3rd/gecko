
#include "parser.h"
#include <base/contract.h>

////////////////////////////////////////

parser::parser( const std::string &n )
	: _func( n )
{
}

////////////////////////////////////////

void parser::parse( std::istream &in )
{
	parse_text( in );
}

////////////////////////////////////////

void parser::save( std::ostream &out )
{
	out << "#include <string>\n";
	for ( auto &i: _includes )
		out << "#include " << base::trim( i ) << '\n';
	_func.save( out );
}

////////////////////////////////////////

void parser::parse_text( std::istream &in )
{
	while ( !in.eof() && in )
	{
		int c = in.get();
		if ( std::char_traits<char>::not_eof( c ) )
		{
			if ( c == '[' )
			{
				switch ( in.peek() )
				{
					case '[':
						_func.push_text();
						parse_expr( in );
						break;

					case '%':
						_func.push_text( true );
						parse_code( in );
						break;

					case '#':
						_func.push_text( true );
						parse_directive( in );
						break;

					case '/':
						_func.push_text( true );
						parse_comment( in );
						break;

					default:
						_func.add( static_cast<char>( c ) );
						break;
				}
			}
			else
				_func.add( static_cast<char>( c ) );
		}
	}
	_func.push_text();
}

////////////////////////////////////////

void parser::parse_expr( std::istream &in )
{
	precondition( in.get() == '[', "missing '[' to start expression" );
	int count = 0;
	while ( !in.eof() && in )
	{
		int c = in.get();
		if ( std::char_traits<char>::not_eof( c ) )
		{
			if ( c == ']' )
			{
				if ( count == 0 )
				{
					_func.push_expr();
					break;
				}
				_func.add( static_cast<char>( c ) );
				--count;
			}
			else if ( c == '[' )
			{
				_func.add( static_cast<char>( c ) );
				++count;
			}
			else if ( c == '"' )
			{
				_func.add( static_cast<char>( c ) );
				parse_string( in );
			}
			else
				_func.add( static_cast<char>( c ) );
		}
	}
	if ( in.get() != ']' )
		throw_runtime( "missing ']' in expression" );
}

////////////////////////////////////////

void parser::parse_code( std::istream &in )
{
	precondition( in.get() == '%', "missing '%' to start code" );

	// find the first word
	std::string word;
	while ( std::isspace( in.peek() ) )
		in.get();
	while ( std::isalpha( in.peek() ) )
		word.push_back( static_cast<char>( in.get() ) );
	while ( std::isspace( in.peek() ) )
		in.get();

	if ( word == "for" )
	{
		_func.add( "for " );
	}
	else if ( word == "if" )
	{
		_func.add( "if " );
	}
	else if ( word == "else" )
	{
		_func.unindent();
		_func.add( "}\nelse" );
		_func.push_code();
	}
	else if ( word == "code" || word == "endfor" || word == "endif" )
	{
	}
	else
		throw_runtime( "unknown template keyword '{0}'", word );

	int count = 0;
	while ( !in.eof() && in )
	{
		int c = in.get();
		if ( std::char_traits<char>::not_eof( c ) )
		{
			if ( c == ']' )
			{
				if ( count == 0 )
				{
					_func.push_code();
					break;
				}
				_func.add( static_cast<char>( c ) );
				--count;
			}
			else if ( c == '[' )
			{
				_func.add( static_cast<char>( c ) );
				++count;
			}
			else if ( c == '"' )
			{
				_func.add( static_cast<char>( c ) );
				parse_string( in );
			}
			else
				_func.add( static_cast<char>( c ) );
		}
	}

	if ( word == "for" || word == "if" || word == "else" )
	{
		_func.add( "\n{" );
		_func.indent();
	}
	else if ( word == "endfor" || word == "endif" )
	{
		_func.unindent();
		_func.add( ']' );
		_func.push_code();
	}
}

////////////////////////////////////////

void parser::parse_directive( std::istream &in )
{
	precondition( in.get() == '#', "missing '#' to start code" );

	// find the first word
	std::string word;
	while ( std::isspace( in.peek() ) )
		in.get();
	while ( std::isalpha( in.peek() ) )
		word.push_back( static_cast<char>( in.get() ) );
	while ( std::isspace( in.peek() ) )
		in.get();

	int count = 0;
	std::string rest;
	while ( !in.eof() && in )
	{
		int c = in.get();
		if ( std::char_traits<char>::not_eof( c ) )
		{
			if ( c == ']' )
			{
				if ( count == 0 )
				{
					break;
				}
				rest.push_back( static_cast<char>( c ) );
				--count;
			}
			else if ( c == '[' )
			{
				rest.push_back( static_cast<char>( c ) );
				++count;
			}
			else if ( c == '"' )
			{
				rest.push_back( static_cast<char>( c ) );
				bool escaped = false;
				while ( !in.eof() && in )
				{
					int sc = in.get();
					if ( std::char_traits<char>::not_eof( sc ) )
					{
						rest.push_back( static_cast<char>( sc ) );
						if ( sc == '"' && !escaped )
							break;
						escaped = false;
						if ( sc == '\\' )
							escaped = true;
					}
				}
			}
			else
				rest.push_back( static_cast<char>( c ) );
		}
	}

	if ( word == "include" )
		_includes.push_back( rest );
	else if ( word == "arg" )
		_func.add_arg( rest );
	else if ( word == "function" )
		_func.set_name( rest );
	else
		throw_runtime( "unknown directive '{0}'", word );

	if ( in.peek() == '\n' )
		in.get();
}

////////////////////////////////////////

void parser::parse_comment( std::istream &in )
{
	precondition( in.get() == '/', "missing '/' to start comment" );
	while ( in.peek() == '/' )
		in.get();

	_func.add( '/' );
	_func.add( '/' );
	_func.add( ' ' );

	while ( !in.eof() && in )
	{
		int c = in.get();
		if ( std::char_traits<char>::not_eof( c ) )
		{
			if ( c == ']' )
				break;
			else
				_func.add( static_cast<char>( c ) );
		}
	}
	_func.push_code();
	if ( in.peek() == '\n' )
		in.get();
}

////////////////////////////////////////

void parser::parse_string( std::istream &in )
{
	bool escaped = false;
	while ( !in.eof() && in )
	{
		int c = in.get();
		if ( std::char_traits<char>::not_eof( c ) )
		{
			_func.add( static_cast<char>( c ) );
			if ( c == '"' && !escaped )
				break;
			escaped = false;
			if ( c == '\\' )
				escaped = true;
		}
	}
}

////////////////////////////////////////



