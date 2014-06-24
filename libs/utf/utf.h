
#pragma once

#include <iostream>
#include <stdexcept>

#include "utfcat.h"
#include "utfprop.h"
#include "utfnorm.h"

namespace std
{
	/// @brief Stream a u32string.
	///
	/// Write an std::u32string to a byte stream (converting to UTF-8).
	std::ostream &operator<<( std::ostream &out, const std::u32string &str );
}

namespace utf
{
	/// @brief Exception for most/all UTF errors.
	class error : public std::runtime_error
	{
	public:
		/// @brief Construct error with message
		error( const std::string &msg );

		/// @brief Destructor
		virtual ~error( void ) throw();
	};

	/// @brief Is the code point valid?
	inline bool is_valid_code_point( char32_t cp )
	{
		return ( cp <= 0x10FFFF );
	}

	/// @brief Throw an exception if the code point is invalid.
	inline char32_t check_code_point( char32_t cp )
	{
		if ( !is_valid_code_point( cp ) )
			throw error( "invalid unicode code point" );
		return cp;
	}

	/// @brief Read a code point from UTF-8
	char32_t read_8( std::istream &str );

	/// @brief Read a code point from UTF-16BE 
	char32_t read_16be( std::istream &str );

	/// @brief Read a code point from UTF-16LE 
	char32_t read_16le( std::istream &str );

	/// @brief Read a code point from UTF-32BE 
	char32_t read_32be( std::istream &str );

	/// @brief Read a code point from UTF-32LE 
	char32_t read_32le( std::istream &str );

	/// @brief UTF modes
	enum mode
	{
		UTF8,
		UTF16LE,
		UTF16BE,
		UTF32LE,
		UTF32BE
	};


	/// @brief Iterate over unicode code points.
	class iterator
	{
	public:
		/// @brief Construct UTF iterator for a stream
		iterator( std::istream &str, mode m = UTF8 );

		/// @brief Get current Unicde code point
		inline char32_t operator *() const { return _value; }

		/// @brief Move to the next code point
		iterator &operator++();

		/// @brief Is the iterator valid
		inline operator bool() const { return (bool)_stream; }

		/// @brief The current line number
		inline size_t line_number( void ) const { return _line; }

		/// @brief The current code point within the line
		inline size_t char_number( void ) const { return _char; }

	private:
		mode _mode = UTF8;
		std::istream &_stream;
		char32_t _value = U'\0';
		size_t _line = 1;
		size_t _char = 1;
	};

	/// @bruef Output code point as UTF-8
	void write( std::ostream &out, char32_t cp );

	/// @brief Convert code point to UTF-8
	template<typename insert_it>
	size_t convert_utf8( char32_t cp, insert_it &it )
	{
		if ( cp <= 0x7F )
		{
			*it++ = (char)cp;
			return 1;
		}

		if ( cp <= 0x7FF )
		{
			*it++ = (char)(0xC0|(cp>>6));
			*it++ = (char)(0x80|(cp&0x3F));
			return 2;
		}

		if ( cp <= 0xFFFF )
		{
			*it++ = (char)(0xE0|(cp>>12));
		   	*it++ = (char)(0x80|((cp>>6)&0x3F));
			*it++ = (char)(0x80|(cp&0x3F));
			return 3;
		}

		if ( cp <= 0x10FFFF )
		{
			*it++ = (char)(0xF0|(cp>>18));
			*it++ = (char)(0x80|((cp>>12)&0x3F));
			*it++ = (char)(0x80|((cp>>6)&0x3F));
			*it++ = (char)(0x80|(cp&0x3F));
			return 3;
		}

		throw error( "invalid unicode code point" );
	}

	/// @brief Convert code point to UTF-8
	inline size_t convert_utf8( char32_t cp, std::string &s )
	{
		std::insert_iterator<std::string> it( s, s.end() );
		return convert_utf8( cp, it );
	}

	/// @brief Sort combining characters in canonical order.
	void canonical_order( std::u32string &str );

	/// @brief Sort combining characters in canonical order.
	void canonical_order( char32_t *first, char32_t *last );

	/// @brief Canonical decompose of string
	void canonical_decompose( std::u32string &str );

	/// @brief Compatibility decompose of string
	void compatibility_decompose( std::u32string &str );

	/// @brief Canonical compose of string
	void canonical_compose( std::u32string &str );

	/// @brief Decompose a string into canonical NFD form
	inline void nfd( std::u32string &str )
	{
		canonical_decompose( str );
	}

	/// @brief Compose a string into canonical NFC form
	inline void nfc( std::u32string &str )
	{
		canonical_decompose( str );
		canonical_compose( str );
	}

	/// @brief Decompose a string into compatiblity NFKD form
	inline void nfkd( std::u32string &str )
	{
		compatibility_decompose( str );
	}

	/// @brief Compose a string into compatiblity NFKC form
	inline void nfkc( std::u32string &str )
	{
		compatibility_decompose( str );
		canonical_compose( str );
	}
}

