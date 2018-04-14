//
// Copyright (c) 2015-2016 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "variant.h"
#include <map>
#include <vector>
#include <string>
#include "string_util.h"

namespace base
{

////////////////////////////////////////

class json;

/// @brief JSON string type
typedef std::string json_string;

/// @brief JSON number type
typedef std::pair<double, std::string> json_number;

/// @brief JSON object type
typedef std::map<std::string,json> json_object;

/// @brief JSON array type
typedef std::vector<json> json_array;

/// @brief JSON boolean type
typedef bool json_bool;

/// @brief JSON null type
typedef std::nullptr_t json_null;


////////////////////////////////////////

/// @brief JSON value
class json
{
public:
	/// @brief JSON null constructor
	json( void )
	{
		set<json_null>( json_null( nullptr ) );
	}

	/// @brief JSON string constructor
	json( const json_string &x )
		: _storage( x )
	{
	}

	/// @brief JSON string constructor
	json( json_string &&x )
		: _storage( std::move( x ) )
	{
	}

	/// @brief JSON object constructor
	json( const json_object &x )
		: _storage( x )
	{
	}

	/// @brief JSON array constructor
	json( const json_array &x )
		: _storage( x )
	{
	}

	/// @brief JSON object move constructor
	json( json_object &&x )
		: _storage( std::move( x ) )
	{
	}

	/// @brief JSON array move constructor
	json( json_array &&x )
		: _storage( std::move( x ) )
	{
	}

	/// @brief JSON number constructor
	json( const json_number &x )
	{
		set<json_number>( x );
	}

	/// @brief JSON number constructor
	json( json_number &&x )
	{
		set<json_number>( std::move( x ) );
	}

	json( double x )
	{
		set<json_number>( std::make_pair( x, to_string( x ) ) );
	}

	/// @brief JSON integer constructor
	json( int x )
	{
		set<json_number>( std::make_pair( static_cast<double>( x ), to_string( x ) ) );
	}

	/// @brief JSON string constructor
	json( const char *x )
	{
		if ( x )
			set<json_string>( json_string( x ) );
		else
			set<json_string>( json_string() );
	}

	/// @brief JSON boolean constructor
	json( json_bool x )
	{
		set<json_bool>( x );
	}

	/// @brief JSON null constructor
	json( json_null x )
	{
		set<json_null>( x );
	}

	/// @brief pass through check to storage
	template <typename X>
	inline bool is( void ) const
	{
		return _storage.is<X>();
	}

	inline bool valid( void ) const
	{
		return _storage.valid();
	}

	inline explicit operator bool( void ) const
	{
		return valid() && ! is<json_null>();
	}

	template <typename X, typename... Args>
	void set( Args &&...args )
	{
		_storage.set<X>( std::forward<Args>( args )... );
	}

	template <typename X>
	X &get( void )
	{
		try
		{
			return _storage.get<X>();
		}
		catch ( std::bad_cast & )
		{
			throw_runtime( "unable to extract type '{0}' from json entry, actual type is '{1}'", typeid(X).name(), _storage.type_name() );
		}
		catch ( ... )
		{
			throw;
		}
	}

	template <typename X>
	const X &get( void ) const
	{
		try
		{
			return _storage.get<X>();
		}
		catch ( std::bad_cast & )
		{
			throw_runtime( "unable to extract type '{0}' from json entry, actual type is '{1}'", typeid(X).name(), _storage.type_name() );
		}
		catch ( ... )
		{
			throw;
		}
	}

	void clear( void )
	{
		return _storage.clear();
	}

	const char *type_name( void ) const
	{
		return _storage.type_name();
	}

	/// @brief Parse a json value from the given string.
	void parse( const std::string &str );

	/// @brief Parse a json value from the given stream.
	void parse( std::istream &in );

	/// @brief Size of string, array, or object.
	///
	/// If this value is of any other type, 0 is returned.
	size_t size( void ) const;

	/// @brief Return json value with the given name.
	///
	/// This value must be an object.
	const json &operator[]( const std::string &name ) const;

	/// @brief Return JSON value with the given name.
	///
	/// This value will become a JSON object if it is currently null.
	/// Otherwise, this value must be an object.
	/// The value will be created and added as a null value if it does not exist.
	json &operator[]( const std::string &name );

	/// @brief Return json value with the given index.
	///
	/// This value must be an array.
	const json &operator[]( size_t idx ) const;

	/// @brief Return json value with the given index.
	///
	/// This value must be an array.
	json &operator[]( size_t idx );

	/// @brief Return json value with the given name.
	///
	/// This value must be an object.
	const json &at( const std::string &name ) const;

	/// @brief Return JSON value with the given name.
	///
	/// This value will become a JSON object if it is currently null.
	/// Otherwise, this value must be an object.
	/// The value will be created and added as a null value if it does not exist.
	json &at( const std::string &name );

	/// @brief Return json value with the given index.
	///
	/// This value must be an array.
	const json &at( size_t idx ) const;

	/// @brief Return json value with the given index.
	///
	/// This value must be an array.
	json &at( size_t idx );

	/// @brief Return whether json object has this key
	///
	/// This value must be an object.
	bool has( const std::string &name ) const;

	/// @brief Append JSON value to end of array.
	///
	/// This value will become a JSON array if it is currently null.
	/// Otherwise, this value must be an array.
	void push_back( const json &x );

	/// @brief Move the JSON value to end of array.
	///
	/// This value will become a JSON array if it is currently null.
	/// Otherwise, this value must be an array.
	void push_back( json &&x );

	/// @brief Push null value to end of array.
	///
	/// This value will become a JSON array if it is currently null.
	/// Otherwise, this value must be an array.
	json &push_back( void );

	void append( json &&that );

	json &append( const std::string &name, json &&val )
	{
		this->operator[]( name ) = std::move( val );
		return *this;
	}

	void dump( void ) const;

	static json create( const std::string &s )
	{
		json result;
		result.parse( s );
		return result;
	}

private:
	void parse_value( std::istream_iterator<char> &it, std::istream_iterator<char> &end, int &line );
	void parse_array( std::istream_iterator<char> &it, std::istream_iterator<char> &end, int &line );
	void parse_object( std::istream_iterator<char> &it, std::istream_iterator<char> &end, int &line );
	void parse_string( std::istream_iterator<char> &it, std::istream_iterator<char> &end, int &line );
	void parse_true( std::istream_iterator<char> &it, std::istream_iterator<char> &end, int &line );
	void parse_false( std::istream_iterator<char> &it, std::istream_iterator<char> &end, int &line );
	void parse_null( std::istream_iterator<char> &it, std::istream_iterator<char> &end, int &line );
	void parse_number( std::istream_iterator<char> &it, std::istream_iterator<char> &end, int &line );

	void skip_whitespace( std::istream_iterator<char> &it, std::istream_iterator<char> &end, int &line );

	variant<json_string,json_number,json_bool,json_array,json_object,json_null> _storage;
};

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const json &j );
void operator<<( std::string &out, const json &j );

////////////////////////////////////////

inline std::istream &operator>>( std::istream &in, json &j )
{
	j.parse( in );
	return in;
}

////////////////////////////////////////

inline void operator>>( const std::string &in, json &j )
{
	j.parse( in );
}

////////////////////////////////////////

}

