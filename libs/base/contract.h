// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include <cassert>
#include <stdexcept>
#include <system_error>
#include "format.h"
#include "likely.h"

namespace base
{

////////////////////////////////////////

/// @brief Exception describing the source file/line
/// @sa throw_location
/// @sa throw_add_location
class location_exception : public std::exception
{
public:
	/// @brief Constructor
	location_exception( const char *file, int line );

	~location_exception( void ) override;
	location_exception( const location_exception &e ) = default;
	location_exception( location_exception &&e ) noexcept(true) = default;
	location_exception &operator=( const location_exception &e ) = default;
	location_exception &operator=( location_exception &&e ) noexcept(true) = default;

	/// @brief Get the source file location.
	const char *file( void ) const
	{
		return _file;
	}

	/// @brief Get the source line location.
	int line( void ) const
	{
		return _line;
	}

	/// @brief Get the exception message
	const char *what( void ) const noexcept override;

private:
	int _line;
	const char *_file;
	std::string _msg;
};

////////////////////////////////////////

/// @brief Print an exception, possible nested
void print_exception( std::ostream &out, const std::exception &e, int level = 0 );

inline std::ostream &operator<<( std::ostream &out, const std::exception &e )
{
	print_exception( out, e );
	return out;
}

////////////////////////////////////////

/// @brief Precondition exception
class precondition_error : public std::logic_error
{
public:
	~precondition_error( void ) override;
	precondition_error( const precondition_error &e ) = default;
	precondition_error( precondition_error &&e ) noexcept(true) = default;
	precondition_error &operator=( const precondition_error &e ) = default;
	precondition_error &operator=( precondition_error &&e ) noexcept(true) = default;
	using std::logic_error::logic_error;
};

////////////////////////////////////////

/// @brief Postcondition exception
class postcondition_error : public std::logic_error
{
public:
	~postcondition_error( void ) override;
	postcondition_error( const postcondition_error &e ) = default;
	postcondition_error( postcondition_error &&e ) noexcept(true) = default;
	postcondition_error &operator=( const postcondition_error &e ) = default;
	postcondition_error &operator=( postcondition_error &&e ) noexcept(true) = default;
	using std::logic_error::logic_error;
};

////////////////////////////////////////

/// @name Macros
/// Helper macros for throw exceptions.
/// @{

namespace detail
{

template <typename E>
[[noreturn]] inline void throw_location_impl( E &&e, const char *f, int l )
{
	try { throw location_exception( f, l ); }
	catch ( ... ) { std::throw_with_nested( std::forward<E>( e ) ); }
}

template <typename ... Args>
[[noreturn]] inline void throw_add_location_impl( const char *f, int l, Args &&... a )
{
	try { std::throw_with_nested( location_exception( f, l ) ); }
	catch ( ... )
	{
		std::throw_with_nested( std::runtime_error( format( std::forward<Args>( a )... ) ) );
	}
}

} // namespace detail

/// @brief Throw the exception, adding it's source location
/// @param exc Exception to add source location to
/// @sa base::location_exception
#define throw_location( exc ) ::base::detail::throw_location_impl( exc, __FILE__, __LINE__ )

/// @brief Add a message as a nested exception
///
/// Can only be called from inside a catch block.
/// @param formats Format arguments.
#define throw_add( ... ) \
	std::throw_with_nested( std::runtime_error( ::base::format( __VA_ARGS__ ) ) );

/// @brief Add the source location as a nested exception.
///
/// Can only be called from inside a catch block.
/// @sa base::location_exception
#define throw_add_location( ... ) \
	::base::detail::throw_add_location_impl( __FILE__, __LINE__, __VA_ARGS__ )

/// @brief Throw a runtime error using format
/// @sa base::format
#define throw_runtime( ... ) \
	throw_location( std::runtime_error( ::base::format( __VA_ARGS__ ) ) )

/// @brief Throw a logic error using format
/// @sa base::format
#define throw_logic( ... ) \
	throw_location( std::logic_error( ::base::format( __VA_ARGS__ ) ) )

/// @brief If check is not true, throw a postcondition_error
#define throw_not_yet() \
	throw_location( std::logic_error( "not yet implemented" ) )

#ifdef _WIN32
/// @brief Throw the last system error using format
/// @sa base::format
/// custom for windows, assumes windows.h has previously
/// been included
# define throw_lasterror( ... ) \
	throw_location( std::system_error( GetLastError(), std::generic_category(), ::base::format( __VA_ARGS__ ) ) )
/// @brief Throw a errno error using format
/// @sa base::format
/// windows puts errno errors in the generic category
# define throw_errno( ... ) \
	throw_location( std::system_error( errno, std::generic_category(), ::base::format( __VA_ARGS__ ) ) )
#else
/// @brief Throw a errno error using format
/// @sa base::format
# define throw_errno( ... ) \
	throw_location( std::system_error( errno, std::system_category(), ::base::format( __VA_ARGS__ ) ) )
#endif

/// @brief If check is not true, throw a precondition_error
#define precondition( check, ... ) \
	( GK_LIKELY( static_cast<bool>(check) ) ? void(0) : throw_location( ::base::precondition_error( ::base::format( __VA_ARGS__ ) ) ) )

/// @brief If check is not true, throw a postcondition_error
#define postcondition( check, ... ) \
	( GK_LIKELY( static_cast<bool>(check) ) ? void(0) : throw_location( ::base::postcondition_error( ::base::format( __VA_ARGS__ ) ) ) )

/// @brief If check is not true, throw a postcondition_error
#define logic_check( check, ... ) \
	( GK_LIKELY( static_cast<bool>(check) ) ? void(0) : throw_location( std::logic_error( ::base::format( __VA_ARGS__ ) ) ) )

#define unused( ... ) \
	(void)( __VA_ARGS__ )

/// @}

////////////////////////////////////////

} // namespace base

