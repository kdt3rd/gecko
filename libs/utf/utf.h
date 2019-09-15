// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "utfcat.h"
#include "utfnorm.h"
#include "utfprop.h"

#include <iostream>
#include <stdexcept>

namespace std
{
/// @brief Stream a u32string.
///
/// Write an std::u32string to a byte stream (converting to UTF-8).
std::ostream &operator<<( std::ostream &out, const std::u32string &str );
} // namespace std

namespace utf
{
/// @brief Exception for most/all UTF errors.
///
/// Error class used for invalid code points and other errors.
class error : public std::runtime_error
{
public:
    /// @brief Construct error with message
    error( const std::string &msg );
    error( const error & ) = default;
    error( error && )      = default;
    error &operator=( const error & ) = default;
    error &operator=( error && ) = default;

    /// @brief Destructor
    virtual ~error( void ) noexcept;
};

/// @brief Is the code point valid?
/// @param cp Code point to test.
/// @return True if the code point is valid.
inline bool is_valid_code_point( char32_t cp ) { return ( cp <= 0x10FFFF ); }

/// @brief Ensure the code point is valid.
/// @param cp Code point to test.
/// @return The same code point passed in.
///
/// An exception will be thrown if the code point is invalid.
inline char32_t check_code_point( char32_t cp )
{
    if ( !is_valid_code_point( cp ) )
        throw error( "invalid unicode code point" );
    return cp;
}

/// @brief Read a code point from UTF-8
/// @param start Start of string.
/// @param end End of string.
/// @return The code point read from the string.
///
/// Read a UTF-8 code point from the string and return it.
char32_t read_8( const char *&start, const char *end );

/// @brief Read a code point from UTF-8
/// @param str Stream to read from.
/// @return The code point read from the stream.
///
/// Read a UTF-8 code point from the stream and return it.
char32_t read_8( std::istream &str );

/// @brief Read a code point from UTF-16BE
/// @param str Stream to read from.
/// @return The code point read from the stream.
///
/// Read a UTF-16BE code point from the stream and return it.
char32_t read_16be( std::istream &str );

/// @brief Read a code point from UTF-16LE
/// @param str Stream to read from.
/// @return The code point read from the stream.
///
/// Read a UTF-16LE code point from the stream and return it.
char32_t read_16le( std::istream &str );

/// @brief Read a code point from UTF-32BE
/// @param str Stream to read from.
/// @return The code point read from the stream.
///
/// Read a UTF-32BE code point from the stream and return it.
char32_t read_32be( std::istream &str );

/// @brief Read a code point from UTF-32LE
/// @param str Stream to read from.
/// @return The code point read from the stream.
///
/// Read a UTF-32LE code point from the stream and return it.
char32_t read_32le( std::istream &str );

/// @brief UTF modes
enum mode
{
    UTF8,    ///< UTF-8 mode
    UTF16LE, ///< UTF-16 little endian mode
    UTF16BE, ///< UTF-16 big endian mode
    UTF32LE, ///< UTF-32 little endian mode
    UTF32BE  ///< UTF-32 big endian mode
};

/// @brief Iterate over unicode code points.
///
/// Code points are parsed from the given stream and UTF mode.
class iterator
{
public:
    /// @brief Construct UTF iterator.
    /// @param str Stream to read from.
    /// @param m UTF mode to use when parsing.
    ///
    /// A reference to the stream is retained, therefore the stream must remain valid for the lifetime of the iterator.
    iterator( std::istream &str, mode m = UTF8 );

    /// @brief Get current Unicode code point.
    /// @return The current code point.
    inline char32_t operator*() const { return _value; }

    /// @brief Move to the next code point
    /// @return The iterator itself.
    ///
    /// Parse the next code point in the stream.
    iterator &operator++();

    /// @brief Is the iterator valid?
    /// @return True if the underlying stream is valid.
    inline operator bool() const { return static_cast<bool>( _stream ); }

    /// @brief The current line number
    /// @return The current line number
    ///
    /// The current line number, starting with 1.
    inline size_t line_number( void ) const { return _line; }

    /// @brief The offset of the current code point.
    /// @return The offset of the current code point.
    ///
    /// The offset of the current code point within the current line.
    /// The first code point is numbered 1.
    inline size_t char_number( void ) const { return _char; }

private:
    mode          _mode = UTF8;
    std::istream &_stream;
    char32_t      _value = U'\0';
    size_t        _line  = 1;
    size_t        _char  = 1;
};

/// @brief Output code point as UTF-8.
/// @param out Stream to write to.
/// @param cp Code point to write out.
///
/// The code point is converted to UTF-8 and written to the stream.
void write( std::ostream &out, char32_t cp );

/// @brief Convert code point to UTF-8
/// @param cp Unicode code point to convert
/// @param it Iterator used to add characters
/// @returns The number of characters added to the string.
///
/// Insert the UTF-8 representation of *cp* using *it*
template <typename Iterator> size_t convert_utf8( char32_t cp, Iterator &it )
{
    typedef typename Iterator::container_type::value_type value_type;

    if ( cp <= 0x7F )
    {
        *it++ = static_cast<value_type>( cp );
        return 1;
    }

    if ( cp <= 0x7FF )
    {
        *it++ = static_cast<value_type>( 0xC0 | ( cp >> 6 ) );
        *it++ = static_cast<value_type>( 0x80 | ( cp & 0x3F ) );
        return 2;
    }

    if ( cp <= 0xFFFF )
    {
        *it++ = static_cast<value_type>( 0xE0 | ( cp >> 12 ) );
        *it++ = static_cast<value_type>( 0x80 | ( ( cp >> 6 ) & 0x3F ) );
        *it++ = static_cast<value_type>( 0x80 | ( cp & 0x3F ) );
        return 3;
    }

    if ( cp <= 0x10FFFF )
    {
        *it++ = static_cast<value_type>( 0xF0 | ( cp >> 18 ) );
        *it++ = static_cast<value_type>( 0x80 | ( ( cp >> 12 ) & 0x3F ) );
        *it++ = static_cast<value_type>( 0x80 | ( ( cp >> 6 ) & 0x3F ) );
        *it++ = static_cast<value_type>( 0x80 | ( cp & 0x3F ) );
        return 4;
    }

    throw error( "invalid unicode code point" );
}

/// @brief Convert code point to UTF-8
/// @param cp Unicode code point to convert
/// @param s String to append UTF-8 to
/// @returns The number of characters added to the string.
///
/// Append the UTF-8 representation of *cp* to string *s*.
inline size_t convert_utf8( char32_t cp, std::string &s )
{
    std::insert_iterator<std::string> it( s, s.end() );
    return convert_utf8( cp, it );
}

/// @brief Sort combining characters in canonical order.
/// @param str String to sort.
///
/// Combining characters will be reordered to be in the canonical order.
/// The string is sorted in-place.
void canonical_order( std::u32string &str );

/// @brief Sort combining characters in canonical order.
/// @param first start of string to sort.
/// @param last end of string to sort.
///
/// Combining characters will be reordered to be in the canonical order.
/// The string is sorted in-place.
void canonical_order( char32_t *first, char32_t *last );

/// @brief Canonical compose of string
/// @param str String to compose.
///
/// Composing means that combining characters are combined into single characters when possible.
/// For example, the character e followed by the combining accent character ´ can be composed into é.
/// The string is composed in-place.
void canonical_compose( std::u32string &str );

/// @brief Canonical decompose of string
/// @param str String to decompose.
///
/// Decomposing means that single characters are expanded with combining characters when possible.
/// For example, é can be decomposed into the character e followed by the combining accent character ´.
///
/// The string is decomposed in-place.
/// @sa compatibility_decompose
void canonical_decompose( std::u32string &str );

/// @brief Compatibility decompose of string
/// @param str String to decompose.
///
/// Decomposing means that single characters are expanded with combining characters when possible.
/// For example, é can be decomposed into the character e followed by the combining accent character ´.
///
/// Compatible sequences are considered to have the same meaning in some context.  For example the ligature ﬀ and ff are different, but can be considered
/// the same in some context.  Canonical sequences are always equivalent.
///
/// The string is decomposed in-place.
/// @sa canonical_decompose
void compatibility_decompose( std::u32string &str );

/// @brief Decompose a string into canonical NFD form
/// @param str String to decompose.
///
/// The string is decomposed in-place.
inline void nfd( std::u32string &str ) { canonical_decompose( str ); }

/// @brief Compose a string into canonical NFC form
/// @param str String to compose.
///
/// The string is composed in-place.
inline void nfc( std::u32string &str )
{
    canonical_decompose( str );
    canonical_compose( str );
}

/// @brief Decompose a string into compatiblity NFKD form
/// @param str String to decompose.
///
/// The string is decomposed in-place.
inline void nfkd( std::u32string &str ) { compatibility_decompose( str ); }

/// @brief Compose a string into compatiblity NFKC form
/// @param str String to compose.
///
/// The string is composed in-place.
inline void nfkc( std::u32string &str )
{
    compatibility_decompose( str );
    canonical_compose( str );
}
} // namespace utf
