
#pragma once

#include <string>
#include <iostream>
#include <iterator>
#include <sstream>
#include <tuple>
#include <vector>
#include <string>
#include <stdexcept>
#include "ansi.h"
#include "string_util.h"

namespace base
{

////////////////////////////////////////

namespace detail
{

/// @brief Parse a format specifier
class format_specifier
{
public:
	/// @brief Parse the format specifier given.
	format_specifier( const char *&fmt, const char *end );

	/// @brief Index of specifier.
	int index;

	/// @brief Width of specifier.
	int width;

	/// @brief Base for numbers.
	int radix;

	/// @brief Precision for numbers.
	int precision;

	/// @brief Alignment withing width.
	int alignment;

	/// @brief Fill character.
	char fill;

	/// @brief Use uppercase for numbers with base > 10.
	bool upper_case;

	/// @brief Show plus sign for positive numbers.
	bool show_plus;

	/// @brief Apply output stream settings for this specifier.
	void apply( std::ostream &out );

private:
	static int parse_number( const char * &fmt, const char *end );
};

}

////////////////////////////////////////

/// @brief Holds a format string with attached arguments.
template<typename ... Args>
class format_holder
{
public:
	/// @brief Construct format holder.
	format_holder( std::string fmt, const Args &...args )
		: _fmt( std::move( fmt ) ), _args( std::tie( args... ) )
	{
	}

	/// @brief Convert to a string.
	operator std::string() const
	{
		std::ostringstream str;
		write( str );
		return str.str();
	}

	/// @brief Write the string to output stream.
	template <typename CharT>
	void write( std::basic_ostream<CharT> &out ) const
	{
		const char *start = format_begin();
		const char *end = format_end();
		const char *cur = start;
		const char *prev = start;

		try
		{
			while ( begin( cur, end ) )
			{
				out.write( prev, int( cur - prev ) );
				std::ios::fmtflags flags( out.flags() );

				detail::format_specifier spec( cur, end );
				spec.apply( out );

				output( out, size_t(spec.index) );
				prev = cur + 1;

				out.flags( flags );
			}
			out.write( prev, int( cur - prev ) );
		}
		catch ( ... )
		{
			std::string tmp( start, end );
			size_t errpos = static_cast<size_t>( cur - start );
			tmp.insert( errpos + 1, ansi::reset );
			tmp.insert( errpos, ansi::invert );
			tmp = replace( std::move( tmp ), '\n', "\\n" );
			std::throw_with_nested( std::runtime_error( "parse error in format \"" + tmp + '\"' ) );
		}
	}

private:
	template <typename CharT>
	void output( std::basic_ostream<CharT> &out, size_t x ) const
	{
		get_arg<CharT, 0, std::tuple_size<std::tuple<Args...>>::value>::output( out, _args, x );
	}

	const char *format_begin( void ) const { return _fmt.c_str(); }
	const char *format_end( void ) const { return _fmt.c_str() + _fmt.size(); }

	template <typename CharT, size_t I, size_t N>
	struct get_arg
	{
		typedef get_arg<CharT,I+1,N-1> base_class;

		template<typename Tuple>
		static void output( std::basic_ostream<CharT> &out, const Tuple &t, size_t x )
		{
			if ( x == I )
			{
				out << std::get<I>( t );
				return;
			}
			base_class::output( out, t, x );
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

	bool begin( const char * &fmt, const char *end ) const
	{
		while ( fmt != end && *fmt != '{' )
			++fmt;
		return fmt != end;
	}


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
///   - # Comment, can be followed by anything (except a closing curly brace).
///
/// @param fmt Format string with specifiers for the given data.
/// @param data Data to format.
/// @return An object that can be converted/casted to a string, or streamed to an std::stream.
///
/// example base/ex_format.cpp
template<typename ... Args>
format_holder<Args...> format( std::string fmt, const Args &...data )
{
	return format_holder<Args...>( std::move( fmt ), data... );
}

////////////////////////////////////////

/// @brief Stream output operator for format.
template<typename CharT, typename ... Args>
std::basic_ostream<CharT> &operator<<( std::basic_ostream<CharT> &out, const format_holder<Args...> &fmt )
{
	fmt.write( out );
	return out;
}

////////////////////////////////////////

/// @brief Print container values with separator.
/// Commonly used to print vector values separated by a comma.
template<typename Container>
class infix_separated_printer
{
public:
	/// @brief Constructor
	/// A reference to the container is kept, so make sure the container outlives this class.
	infix_separated_printer( std::string sep, const Container &v )
		: _sep( std::move( sep ) ), _container( v )
	{
	}

	/// @brief Write the container values with separator.
	void write( std::ostream &out ) const
	{
		if ( !_container.empty() )
		{
			bool first = true;
			for ( auto &i: _container )
			{
				if ( !first )
					out << _sep << i;
				else
					out << i;
				first = false;
			}
		}
	}

private:
	std::string _sep;
	const Container &_container;
};

////////////////////////////////////////

/// @brief Print infix separated container.
template<typename C>
std::ostream &operator<<( std::ostream &out, const infix_separated_printer<C> &tmp )
{
	tmp.write( out );
	return out;
}

////////////////////////////////////////

/// @brief Create an infix_separated_printer.
/// The easy way to print a container with a separator (e.g. comma).
template<typename C>
infix_separated_printer<C> infix_separated( std::string sep, const C &c )
{
	return infix_separated_printer<C>( std::move( sep ), c );
}

////////////////////////////////////////

}

