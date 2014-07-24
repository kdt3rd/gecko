
#include <iostream>
#include <iomanip>
#include "format.h"
#include "contract.h"

namespace base
{

////////////////////////////////////////

format_specifier::format_specifier( const char * &fmt, const char *end )
	: index( 0 ), width( -1 ), base( 10 ), precision( -1 ), alignment( -1 ), fill( ' ' ), upper_case( false ), show_plus( false )
{
	precondition( fmt != end, "empty format specifier" );
	precondition( *fmt == '{', "format specifier expected '{' (got '{0}')", *fmt );

	++fmt;
	index = parse_number( fmt, end );
	while ( *fmt == ',' )
	{
		++fmt;
		switch ( *fmt )
		{
			case 'w':
				++fmt;
				width = parse_number( fmt, end );
				break;

			case 'b':
				++fmt;
				base = parse_number( fmt, end );
				upper_case = false;
				break;

			case 'B':
				++fmt;
				base = parse_number( fmt, end );
				upper_case = true;
				break;

			case 'f':
				++fmt;
				fill = *fmt;
				++fmt;
				break;

			case '+':
				++fmt;
				show_plus = true;
				break;

			case 'p':
				++fmt;
				precision = parse_number( fmt, end );
				break;

			case 'a':
				++fmt;
				switch ( *fmt )
				{
					case 'l': alignment = -1; break;
					case 'r': alignment = 1; break;
					default: throw_runtime( "unknown alignment (got '{0}')", *fmt );
				}
				++fmt;
				break;

			default:
				throw_runtime( "unknown format specifier (got '{0}')", *fmt );
		}
	}

	if ( *fmt == '#' )
	{
		while ( fmt != end && *fmt != '}' )
			++fmt;
	}

	if ( *fmt != '}' )
		throw_runtime( "format specifier expected ',' or '}' (got '{0}')", *fmt );
}

////////////////////////////////////////

void
format_specifier::apply( std::ostream &out )
{
	if ( width >= 0 )
		out.width( width );

	out.unsetf( std::ios_base::basefield );
	switch ( base )
	{
		case 8: out.setf( std::ios_base::oct ); break;
		case 10: out.setf( std::ios_base::dec ); break;
		case 16: out.setf( std::ios_base::hex ); break;
		default: throw_runtime( "unsupported base: {0}", base );
	}

	out.unsetf( std::ios_base::floatfield );
	if ( precision >= 0 )
	{
		out.setf( std::ios_base::fixed );
		out.precision( precision );
	}
	else
		out.setf( std::ios_base::scientific );

	out.unsetf( std::ios_base::adjustfield );
	if ( alignment < 0 )
		out.setf( std::ios_base::left );
	else if ( alignment > 0 )
		out.setf( std::ios_base::right );

	out.fill( fill );

	if ( upper_case )
		out.setf( std::ios_base::uppercase );
	else
		out.unsetf( std::ios_base::uppercase );

	if ( show_plus )
		out.setf( std::ios_base::showpos );
	else
		out.unsetf( std::ios_base::showpos );
}

////////////////////////////////////////

bool
format_specifier::begin( const char * &fmt, const char *end )
{
	while ( fmt != end && *fmt != '{' )
		++fmt;

	return fmt != end;
}

////////////////////////////////////////

int
format_specifier::parse_number( const char * &fmt, const char *end )
{
	int n = 0;
	int digits = 0;

	while ( fmt != end )
	{
		if ( !isdigit( *fmt ) )
			break;

		n = n * 10 + ( *fmt - '0' );
		digits++;
		fmt++;
	}

	if ( digits == 0 )
		throw_runtime( "expected number (got '{0}')", *fmt );

	return n;
}

////////////////////////////////////////

}

