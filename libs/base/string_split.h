//
// Copyright (c) 2015 Kimball Thurston
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

#pragma once

#include "const_string.h"
#include <vector>
#include <string>
#include <iostream>


////////////////////////////////////////


namespace base
{


////////////////////////////////////////////////////////////////////////////////
// Utility functions to split on either the individual separator or a set of
// separator characters
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////


template <typename stringT, typename Delim, typename Inserter>
inline void
split( const stringT &str, const Delim &delim, Inserter insert, bool skip_empty = false )
{
	if ( skip_empty )
	{
		typename stringT::size_type last = str.find_first_not_of( delim, 0 );
		typename stringT::size_type cur = str.find_first_of( delim, last );

		while ( cur != stringT::npos || last < str.size() )
		{
			if ( cur != last )
				insert = str.substr( last, cur - last );
			last = str.find_first_not_of( delim, cur );
			cur = str.find_first_of( delim, last );
		}
	}
	else
	{
		typename stringT::size_type last = 0;
		typename stringT::size_type cur = str.find_first_of( delim, last );

		do
		{
			if ( cur == stringT::npos )
			{
				if ( last == str.size() )
					insert = stringT();
				else
					insert = str.substr( last );
				break;
			}
			else
				insert = str.substr( last, cur - last );

			last = cur + 1;
			cur = str.find_first_of( delim, last );
		} while ( last != stringT::npos );
	}
}

//template<typename Inserter>
//inline void
//split( const std::string &str, char delim, Inserter insert )
//{
//	/// @todo Implement this in a more efficient way.
//	std::stringstream ss( str );
//	std::string item;
//	while ( std::getline( ss, item, delim ) )
//		insert = item;
//}


////////////////////////////////////////


template <typename stringT>
inline void
split( std::vector<stringT> &ret, const stringT &str, const stringT &sep, bool skip_empty = false )
{
	ret.clear();

	split( str, sep, std::back_inserter( ret ), skip_empty );
}


////////////////////////////////////////


template <typename stringT>
inline void
split( std::vector<stringT> &ret, const stringT &str,
	   const typename stringT::value_type *sep,
	   bool skip_empty = false )
{
	split( ret, str, stringT( sep ), skip_empty );
}


////////////////////////////////////////


template <typename stringT>
inline std::vector<stringT>
split( const stringT &str, const stringT &sep, bool skip_empty = false )
{
	std::vector<stringT> retval;
	if ( sep.size() == 1 )
		split( str, sep[0], std::back_inserter( retval ), skip_empty );
	else
		split( str, sep, std::back_inserter( retval ), skip_empty );
	return std::move( retval );
}


////////////////////////////////////////


template <typename stringT, std::size_t sepSz>
inline std::vector<stringT>
split( const stringT &str, const typename stringT::value_type (&sep)[sepSz],
	   bool skip_empty = false )
{
	std::vector<stringT> retval;
	if ( sepSz == 2 )
		split( str, sep[0], std::back_inserter( retval ), skip_empty );
	else
		split( str, stringT( sep, sepSz - 1 ), std::back_inserter( retval ), skip_empty );
	return std::move( retval );
}


////////////////////////////////////////


template <typename stringT>
inline std::vector<stringT>
split( const stringT &str, const typename stringT::value_type *sep,
	   bool skip_empty = false )
{
	return split( str, stringT( sep ), skip_empty );
}


////////////////////////////////////////


template <typename stringT>
inline void
split( std::vector<stringT> &ret, const stringT &str,
	   typename stringT::value_type sep,
	   bool skip_empty = false )
{
	ret.clear();
	split( str, sep, std::back_inserter( ret ), skip_empty );
}


////////////////////////////////////////


template <typename stringT>
inline std::vector<stringT>
split( const stringT &str, typename stringT::value_type sep,
	   bool skip_empty = false )
{
	std::vector<stringT> ret;
	split( str, sep, std::back_inserter( ret ), skip_empty );
	return std::move( ret );
}


////////////////////////////////////////


} // namespace base



