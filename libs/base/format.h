
#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <tuple>
#include <stdexcept>
#include "ansi.h"
#include "string_util.h"

namespace base
{

////////////////////////////////////////

template<typename ... Args>
class format_holder
{
public:
	format_holder( std::string fmt, const Args &...args )
		: _fmt( std::move( fmt ) ), _args( std::tie( args... ) )
	{
	}

	operator std::string()
	{
		std::ostringstream str;
		str << *this;
		return str.str();
	}

	template <typename CharT>
	void output( std::basic_ostream<CharT> &out, size_t x ) const
	{
		get_arg<CharT, 0, std::tuple_size<std::tuple<Args...>>::value>::output( out, _args, x );
	}

	const char *format_begin( void ) const { return _fmt.c_str(); }
	const char *format_end( void ) const { return _fmt.c_str() + _fmt.size(); }

private:
	template <typename CharT, size_t I, size_t N>
	struct get_arg
	{
		typedef get_arg<CharT,I+1,N-1> base;

		template<typename Tuple>
		static void output( std::basic_ostream<CharT> &out, const Tuple &t, size_t x )
		{
			if ( x == I )
			{
				out << std::get<I>( t );
				return;
			}
			base::output( out, t, x );
		}
	};

	template <typename CharT, size_t I>
	struct get_arg<CharT,I,0>
	{
		template<typename Tuple>
		static void output( std::basic_ostream<CharT> &, const Tuple &, size_t )
		{
			throw std::runtime_error( "Invalid fmt format string or missing argument" );
		}
	};

	std::string _fmt;
	std::tuple<const Args&...> _args;
};

////////////////////////////////////////

/// @brief Write data into a string with the given format.
///
/// Format the data into a string of the form given by fmt according to format specifiers in the fmt string.
///
/// A format specifier is a decimal number enclosed in curly braces, with optional formatting commands.
/// The number indicates which data to format at that position in the string.
/// The commands are single letters followed by parameters.  Allowed commands are:
///   - w Width (number of characters to use).
///   - b Base for numbers, using lower case.
///   - B Base for numbers, using upper case.
///   - f Fill character to use when padding.
///   - + Show plus sign for positive numbers.
///   - p Precision for floating point numbers.
///   - a Alignment, left or right (followed by a single letter l or r).
///   - # Comment, can be followed by anything (except a closing }).
///
/// @param fmt Format string with specifiers for the given data.
/// @param data Data to format.
/// @return An object that can be converted/casted to a string, or streamed to an std::stream.
///
/// @example test_format.cpp
template<typename ... Args>
format_holder<Args...> format( std::string fmt, const Args &...data )
{
	return format_holder<Args...>( std::move( fmt ), data... );
}

////////////////////////////////////////

class format_specifier
{
public:
	/// Parse the format specifier given.
	format_specifier( const char *&fmt, const char *end );

	int index;
	int width;
	int base;
	int precision;
	int alignment;
	char fill;
	bool upper_case;
	bool show_plus;

	void apply( std::ostream &out );

	static bool begin( const char * &fmt, const char *end );

private:
	static int parse_number( const char * &fmt, const char *end );
};

////////////////////////////////////////

template<typename CharT, typename ... Args>
std::basic_ostream<CharT> &operator<<( std::basic_ostream<CharT> &out, const format_holder<Args...> &fmt )
{
	const char *start = fmt.format_begin();
	const char *end = fmt.format_end();
	const char *cur = start;
	const char *prev = start;

	try
	{
		while ( format_specifier::begin( cur, end ) )
		{
			out.write( prev, int( cur - prev ) );
			std::ios::fmtflags flags( out.flags() );

			format_specifier spec( cur, end );
			spec.apply( out );

			fmt.output( out, size_t(spec.index) );
			prev = cur + 1;

			out.flags( flags );
		}
		out.write( prev, int( cur - prev ) );
	}
	catch ( ... )
	{
		std::string tmp( start, end );
		size_t errpos = cur - start;
		tmp.insert( errpos + 1, ansi::reset );
		tmp.insert( errpos, ansi::invert );
		tmp = replace( std::move( tmp ), '\n', "\\n" );
		std::throw_with_nested( std::runtime_error( "parse error in format \"" + tmp + '\"' ) );
	}

	return out;
}

////////////////////////////////////////

}

