//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include "string_split.h"
#include "contract.h"
#include <cctype>


////////////////////////////////////////


namespace base
{

////////////////////////////////////////

bool
find_unix_vars( std::vector<std::string> &pieces, std::vector<std::string> &vars, const std::string &str )
{
	std::string curpiece;
	std::string::size_type cur = 0;
	std::string::size_type i = str.find_first_of( '%' );
	std::string::size_type last = str.size();

	while ( cur != last )
	{
		if ( str[cur] == '$' )
		{
			if ( cur > 0 && str[cur-1] == '\\' )
			{
				curpiece.back() = '$';
				++cur;
				continue;
			}
			++cur;
			// $ at last char is just $
			if ( cur == last )
			{
				curpiece.push_back( '$' );
				break;
			}

			if ( std::isalpha( str[cur] ) || str[cur] == '_' )
			{
				// plain old variable, possibly with an array at the end
				std::string var( 1, str[cur] );
				++cur;
				while ( cur != last && ( std::isalnum( str[cur] ) || str[cur] == '_' ) )
				{
					var.push_back( str[cur] );
					++cur;
				}
				if ( cur != last && str[cur] == '[' )
				{
					// array deref
					int count = 1;
					var.push_back( '[' );
					while ( count > 0 )
					{
						++cur;
						if ( cur == last )
							break;
						if ( str[cur] == '[' )
							++count;
						else if ( str[cur] == ']' )
							--count;
						var.push_back( str[cur] );
					}

					if ( count == 0 )
					{
						pieces.push_back( curpiece );
						curpiece.clear();
						vars.emplace_back( std::move( var ) );
						++cur;
					}
					else
						throw_runtime( "Unable to find end of array reference with var looking like '{0}' in string '{1}'", var, str );
				}
				else
				{
					pieces.push_back( curpiece );
					curpiece.clear();
					vars.emplace_back( std::move( var ) );
				}
			}
			else if ( str[cur] == '{' )
			{
				int count = 1;
				std::string var;
				while ( count > 0 )
				{
					++cur;
					if ( cur == last )
						break;
					if ( str[cur] == '{' )
						++count;
					else if ( str[cur] == '}' )
						--count;
					var.push_back( str[cur] );
				}
				if ( count == 0 )
				{
					pieces.push_back( curpiece );
					curpiece.clear();
					vars.emplace_back( std::move( var ) );
					++cur;
				}
				else
					throw_runtime( "Unable to find end of variable reference with var looking like '{0}' in string '{1}'", var, str );
			}
			else
				throw_runtime( "Invalid variable character at position {0} in string '{1}'", cur, str );
		}
		else
		{
			curpiece.push_back( str[cur] );
			++cur;
		}
	}

	if ( ! curpiece.empty() )
		pieces.emplace_back( std::move( curpiece ) );

	return ! vars.empty();
}


////////////////////////////////////////


std::string
replace_unix_vars( const std::string &str,
				   const std::function<std::string(const std::string &)> &varLookup )
{
	if ( str.empty() )
		return std::string();

	if ( ! varLookup )
		throw_logic( "Invalid variable lookup function provided" );

	std::vector<std::string> pieces;
	std::vector<std::string> vars;
	if ( find_unix_vars( pieces, vars, str ) )
	{
		postcondition( ! vars.empty() && ( pieces.size() == vars.size() || pieces.size() == ( vars.size() + 1 ) ), "bug in find_unix_vars with string '{0}', expect N or N+1 pieces for N vars", str );
		std::string retval = std::move( pieces[0] );
		size_t p = 1;
		for ( auto &v: vars )
		{
			retval.append( varLookup( v ) );
			// if the variable was at the end, pieces will be short by one
			if ( p < pieces.size() )
				retval.append( pieces[p++] );
		}
		return retval;
	}

	postcondition( pieces.size() == 1 && vars.empty(), "bug in find_unix_vars with string '{0}'", str );
	return std::move( pieces[0] );
}


////////////////////////////////////////


bool
find_dos_vars( std::vector<std::string> &pieces,
			   std::vector<std::string> &vars,
			   const std::string &str )
{
	pieces.clear();
	vars.clear();
	if ( str.empty() )
		return false;

	std::string::size_type cur = 0;
	std::string::size_type i = str.find_first_of( '%' );
	std::string::size_type last = str.size() - 1;
	bool collapseLast = false;
	while ( i != std::string::npos )
	{
		if ( i == last )
			break;

		std::string::size_type e = str.find_first_of( '%', i + 1 );
		if ( e == std::string::npos )
			throw_runtime( "Unable to find end of variable marker in {0}", str );

		if ( e == ( i + 1 ) )
		{
			std::string p = str.substr( cur, i - cur + 1 );
			if ( collapseLast )
				pieces.back().append( p );
			else
				pieces.emplace_back( std::move( p ) );
			cur = e + 1;
			collapseLast = true;
		}
		else
		{
			std::string p = str.substr( cur, i - cur );
			if ( collapseLast )
				pieces.back().append( p );
			else
				pieces.emplace_back( std::move( p ) );

			vars.emplace_back( str.substr( i + 1, e - i - 1 ) );
			cur = e + 1;
			collapseLast = false;
		}
		i = str.find_first_of( '%', cur );
	}

	if ( cur < str.size() )
	{
		std::string p = str.substr( cur );
		if ( collapseLast )
			pieces.back().append( p );
		else
			pieces.emplace_back( std::move( p ) );
	}

	return ! vars.empty();
}


/// applies dos-like variable substitution (%foo% for value).
/// no support for delayed evaluation has been provided
std::string
replace_dos_vars( const std::string &str,
				  std::function<std::string(const std::string &)> &varLookup )
{
	if ( str.empty() )
		return std::string();

	if ( ! varLookup )
		throw_logic( "Invalid variable lookup function provided" );

	std::string retval;
	std::string::size_type cur = 0;
	std::string::size_type last = str.size();

	while ( cur != last )
	{
		if ( str[cur] == '%' )
		{
			std::string::size_type e = str.find_first_of( '%', cur + 1 );
			if ( e == std::string::npos )
				throw_runtime( "Unable to find end of variable in {0} starting at position {1}", str, cur );
			if ( e == ( cur + 1 ) )
				retval.push_back( '%' );
			else
			{
				std::string v = str.substr( cur + 1, e - cur - 1 );
				retval.append( varLookup( v ) );
			}

			cur = e + 1;
		}
		else
		{
			retval.push_back( str[cur] );
			++cur;
		}
	}

	return retval;
}

////////////////////////////////////////

} // base



