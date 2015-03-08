
#pragma once

#include "variant.h"
#include <map>
#include <vector>
#include <string>

namespace base
{

////////////////////////////////////////

class json;
typedef std::string json_string;
typedef double json_number;
typedef std::map<std::string,json> json_object;
typedef std::vector<json> json_array;
typedef bool json_bool;

class json_null
{
};


////////////////////////////////////////

class json : public variant<json_string,json_number,json_bool,json_array,json_object,json_null>
{
public:
	json( void )
	{
		json_null x;
		set<json_null>( x );
	}

	json( const json_string &x )
		: variant( x )
	{
	}

	json( json_string &&x )
		: variant( std::move( x ) )
	{
	}

	json( const json_object &x )
		: variant( x )
	{
	}

	json( const json_array &x )
		: variant( x )
	{
	}

	json( json_object &&x )
		: variant( std::move( x ) )
	{
	}

	json( json_array &&x )
		: variant( std::move( x ) )
	{
	}

	json( json_number x )
	{
		set<json_number>( x );
	}

	json( const char *x )
	{
		set<std::string>( x );
	}

	json( json_bool x )
	{
		set<json_bool>( x );
	}

	void parse( const std::string &str );
	void parse( std::istream &in );

	size_t size( void ) const;

	const json &operator[]( const std::string &name ) const;
	json &operator[]( const std::string &name );

	const json &operator[]( size_t idx ) const;
	json &operator[]( size_t idx );

	const json &at( const std::string &name ) const;
	json &at( const std::string &name );

	const json &at( size_t idx ) const;
	json &at( size_t idx );

	void push_back( const json &x );
	void push_back( json &&x );

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

	static json error;
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

inline void operator>>( std::string &in, json &j )
{
	j.parse( in );
}

////////////////////////////////////////

}


