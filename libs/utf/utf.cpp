
#include "utf.h"
#include <utfnorm.h>

#include <iomanip>

////////////////////////////////////////

// Helper functions for parsing utf8.
namespace
{
	inline char32_t follow( int b )
	{
	   	if ( ( b & 0xC0 ) != 0x80 )
		   	throw utf::error( "invalid utf8 follow byte" );
	   	return char32_t( b & ~0x80 );
	}

	inline char32_t byte1of2( int b )
	{
	   	return char32_t( int32_t( b & ~0xC0 ) << 6 );
	}

	inline char32_t byte2of2( int b )
   	{
	   	return follow( b );
   	}

	inline char32_t byte1of3( int b )
   	{
	   	return uint32_t( b & ~0xE0 ) << 12;
   	}

	inline char32_t byte2of3( int b )
	{
	   	return follow( b ) << 6;
   	}

	inline char32_t byte3of3( int b )
	{
	   	return follow( b );
   	}

	inline char32_t byte1of4( int b )
	{
	   	return uint32_t( b & ~0xF0 ) << 18;
	}

	inline char32_t byte2of4( int b )
	{
		return follow( b ) << 12;
	}

	inline char32_t byte3of4( int b )
	{
		return follow( b ) << 6;
	}

	inline char32_t byte4of4( int b )
	{
		return follow( b );
   	}
}

////////////////////////////////////////

namespace utf
{

////////////////////////////////////////

char32_t read_8( std::istream &in )
{
	char32_t result = 0;

	int byte = in.get();
	if ( in.eof() )
		return 0;

	if( byte < 0x80 )
		result = char32_t( byte );
	else if ( ( byte >> 5 ) == 0x06 )
	{
		result = byte1of2( byte ) + byte2of2( in.get() );
		if ( in.eof() )
			throw error( "utf8 sequence ended prematurely" );
	}
	else if ( ( byte >> 4 ) == 0x0E )
	{
		result = byte1of3( byte ) + byte2of3( in.get() );
		if ( in.eof() )
			throw error( "utf8 sequence ended prematurely" );

		result += byte3of3( in.get() );
		if ( in.eof() )
			throw error( "utf8 sequence ended prematurely" );
	}
	else if ( ( byte >> 3 ) == 0x1E )
	{
		result = byte1of4( byte ) + byte2of4( in.get() );
		if ( in.eof() )
			throw error( "utf8 sequence ended prematurely" );
		result += byte3of4( in.get() );
		if ( in.eof() )
			throw error( "utf8 sequence ended prematurely" );
		result += byte4of4( in.get() );
		if ( in.eof() )
			throw error( "utf8 sequence ended prematurely" );
	}
	else
		throw error( "invalid utf8 lead byte" );

	return check_code_point( result );
}

////////////////////////////////////////

char32_t read_16be( std::istream &in )
{
	int a = in.get();
	if ( in.eof() )
		return 0;

	int b = in.get();
	if ( in.eof() )
		throw error( "utf16 sequence ended prematurely" );

	char32_t result = char32_t( (a << 8) + b );
	if ( result < 0xD800 || result > 0xDFFF )
		return 0;
	if ( result > 0xDBFF )
		throw error( "invalid utf16 high surrogate" );

	a = in.get();
	if ( in.eof() )
		throw error( "utf16 sequence ended prematurely" );

	b = in.get();
	if ( in.eof() )
		throw error( "utf16 sequence ended prematurely" );

	int w2 = (a << 8) + b;
	if ( w2 < 0xDC00 || w2 > 0xDFFF )
		throw error( "invalid utf16 low surrogate" );
	result = char32_t( 0x10000 + ( (result & 0x3FF) << 10 ) + ( w2 & 0x3FF ) );

	return check_code_point( result );
}

////////////////////////////////////////

char32_t read_16le( std::istream &in )
{
	int a = in.get();
	if ( in.eof() )
		return 0;

	int b = in.get();
	if ( in.eof() )
		throw error( "utf16 sequence ended prematurely" );

	char32_t result = char32_t( (b << 8) + a );
	if ( result < 0xD800 || result > 0xDFFF )
		return 0;
	if ( result > 0xDBFF )
		throw error( "invalid utf16 high surrogate" );

	a = in.get();
	if ( in.eof() )
		throw error( "utf16 sequence ended prematurely" );

	b = in.get();
	if ( in.eof() )
		throw error( "utf16 sequence ended prematurely" );

	int w2 = (b << 8) + a;
	if ( w2 < 0xDC00 || w2 > 0xDFFF )
		throw error( "invalid utf16 low surrogate" );
	result = char32_t( 0x10000 + ( (result & 0x3FF) << 10 ) + ( w2 & 0x3FF ) );

	return check_code_point( result );
}

////////////////////////////////////////

char32_t read_32be( std::istream &in )
{
	int a = in.get();
	if ( in.eof() )
	   	return 0;

	int b = in.get();
	if ( in.eof() )
	   	throw error( "utf32 sequence ended prematurely" );

	int c = in.get();
	if ( in.eof() )
	   	throw error( "utf32 sequence ended prematurely" );

	int d = in.get();
	if ( in.eof() )
	   	throw error( "utf32 sequence ended prematurely" );

	char32_t result = char32_t( (((((a << 8) + b) << 8) + c) << 8) + d );

	return check_code_point( result );
}

////////////////////////////////////////

char32_t read_32le( std::istream &in )
{
	int a = in.get();
	if ( in.eof() )
		return 0;

	int b = in.get();
	if ( in.eof() )
	   	throw error( "utf32 sequence ended prematurely" );

	int c = in.get();
	if ( in.eof() )
	   	throw error( "utf32 sequence ended prematurely" );

	int d = in.get();
	if ( in.eof() )
	   	throw error( "utf32 sequence ended prematurely" );
	char32_t result = char32_t( (((((d << 8) + c) << 8) + b) << 8) + a );

	return check_code_point( result );
}

////////////////////////////////////////

error::error( const std::string &msg )
	: std::runtime_error( msg )
{
}

////////////////////////////////////////

error::~error( void ) throw()
{
}

////////////////////////////////////////

iterator::iterator( std::istream &str, mode m )
	: _mode( m ), _stream( str )
{
}

////////////////////////////////////////

iterator &
iterator::operator++()
{
	char32_t prev = _value;
	switch ( _mode )
	{
		case UTF8:
			_value = read_8( _stream );
			break;

		case UTF16BE:
			_value = read_16be( _stream );
			break;

		case UTF16LE:
			_value = read_16le( _stream );
			break;

		case UTF32BE:
			_value = read_32be( _stream );
			break;

		case UTF32LE:
			_value = read_32le( _stream );
			break;
	}

	switch ( _value )
	{
		case 0x000B: // Vertical tab
		case 0x000C: // Form feed
		case 0x000D: // Carriage return
		case 0x0085: // Next line
		case 0x2028: // Line separator
		case 0x2029: // Paragraph separator
			_line++;
			_char = 0;
			break;

		case 0x000A: // New line
			if ( prev != 0x000D )
			{
				_line++;
				_char = 0;
			}
			break;
	}

	_char++;

	return *this;
}

////////////////////////////////////////

void write( std::ostream &out, char32_t cp )
{
	std::ios::fmtflags saved = out.flags();

	if ( cp <= 0x7F )
		out << (char)cp;
	else if ( cp <= 0x7FF )
		out << (char)(0xC0|(cp>>6)) << (char)(0x80|(cp&0x3F));
	else if ( cp <= 0xFFFF )
		out << (char)(0xE0|(cp>>12)) << (char)(0x80|((cp>>6)&0x3F)) << (char)(0x80|(cp&0x3F));
	else if ( cp <= 0x10FFFF )
		out << (char)(0xF0|(cp>>18)) << (char)(0x80|((cp>>12)&0x3F)) << (char)(0x80|((cp>>6)&0x3F)) << (char)(0x80|(cp&0x3F));
	else
		throw error( "invalid unicode code point" );

	out.flags( saved );
}

////////////////////////////////////////

void canonical_order( std::u32string &str )
{
	canonical_order( &str[0], &str[0] + str.size() );
}

////////////////////////////////////////

void canonical_order( char32_t *first, char32_t *last )
{
	for ( char32_t *c = first + 1; c < last; ++c )
	{
		uint8_t ccc = canonical_combining_class( *c );
		if ( ccc > 0 )
		{
			while ( c > first && ccc < canonical_combining_class( *(c-1) ) )
			{
				std::swap( *c, *(c-1) );
				--c;
			}
		}
	}
}

////////////////////////////////////////

void canonical_decompose( std::u32string &str )
{
	std::u32string tmp;
	for ( size_t i = 0; i < str.size(); ++i )
		canonical_decompose( str[i], tmp );
	using std::swap;
	swap( str, tmp );
}

////////////////////////////////////////

void compatibility_decompose( std::u32string &str )
{
	std::u32string tmp;
	for ( size_t i = 0; i < str.size(); ++i )
		compatibility_decompose( str[i], tmp );
	using std::swap;
	swap( str, tmp );
}

////////////////////////////////////////

void canonical_compose( std::u32string &str )
{
	for ( size_t b = 0; b < str.size(); ++b )
	{
		if ( canonical_combining_class( str[b] ) != 0 )
			continue;

		uint32_t base = compose_base( str[b] );
		if ( base < 0xFFFFFFFF )
		{
			for ( size_t c = b+1; c < str.size(); ++c )
			{
				uint32_t comb = compose_combine( str[c] );

				// If the combining character is "blocked", then skip it
				if ( b+1 != c && canonical_combining_class( str[c-1] ) >= canonical_combining_class( str[c] ) )
					comb = 0xFFFFFFFF;
				if ( comb < 0xFFFFFFFF )
				{
					char32_t subst = compose_char( base, comb );
					if ( subst > 0 )
					{
						str[b] = subst;
						str.erase( str.begin() + ssize_t(c) );
						base = compose_base( str[b] );
						if ( base == 0xFFFFFFFF )
							break;
						--c;
						continue;
					}
				}
				if ( canonical_combining_class( str[c] ) == 0 )
					break;
			}
		}
	}
}

////////////////////////////////////////

} // end of namespace

////////////////////////////////////////

namespace std
{
std::ostream &operator<<( std::ostream &out, const std::u32string &str )
{
	std::ios::fmtflags saved = out.flags();
	for ( size_t i = 0; i < str.size(); ++i )
	{
		if ( str[i] <= 0x7F )
			out << (char)str[i];
		else if ( str[i] <= 0x7FF )
			out << (char)(0xC0|(str[i]>>6)) << (char)(0x80|(str[i]&0x3F));
		else if ( str[i] <= 0xFFFF )
			out << (char)(0xE0|(str[i]>>12)) << (char)(0x80|((str[i]>>6)&0x3F)) << (char)(0x80|(str[i]&0x3F));
		else if ( str[i] <= 0x10FFFF )
			out << (char)(0xF0|(str[i]>>18)) << (char)(0x80|((str[i]>>12)&0x3F)) << (char)(0x80|((str[i]>>6)&0x3F)) << (char)(0x80|(str[i]&0x3F));
		else
			throw utf::error( "invalid unicode code point" );
	}

	out.flags( saved );
	return out;
}
}

////////////////////////////////////////

