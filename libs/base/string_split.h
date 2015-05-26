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


template<typename Inserter>
inline void
split( const std::string &str, char delim, Inserter insert )
{
	/// @todo Implement this in a more efficient way.
	std::stringstream ss( str );
	std::string item;
	while ( std::getline( ss, item, delim ) )
		insert = item;
}


////////////////////////////////////////


template <typename stringT>
inline void
split( std::vector<stringT> &__ret, const stringT &__str, const stringT &__sep, bool __skip_empty = false )
{
	__ret.clear();

	if ( __skip_empty )
	{
		typename stringT::size_type __last = __str.find_first_not_of( __sep, 0 );
		typename stringT::size_type __cur = __str.find_first_of( __sep, __last );
		
		while ( __cur != stringT::npos || __last < __str.size() )
		{
			if ( __cur != __last )
				__ret.push_back( __str.substr( __last, __cur - __last ) );
			__last = __str.find_first_not_of( __sep, __cur );
			__cur = __str.find_first_of( __sep, __last );
		}
	}
	else
	{
		typename stringT::size_type __last = 0;
		typename stringT::size_type __cur = __str.find_first_of( __sep, __last );

		do
		{
			if ( __cur == stringT::npos )
			{
				if ( __last == __str.size() )
					__ret.push_back( stringT() );
				else
					__ret.push_back( __str.substr( __last ) );
				break;
			}
			else
				__ret.push_back( __str.substr( __last, __cur - __last ) );

			__last = __cur + 1;
			__cur = __str.find_first_of( __sep, __last );
		} while ( __last != stringT::npos );
	}
}


////////////////////////////////////////


template <typename stringT>
inline void
split( std::vector<stringT> &__ret, const stringT &__str,
	   const typename stringT::value_type *__sep,
	   bool __skip_empty = false )
{
	split( __ret, __str, stringT( __sep ), __skip_empty );
}


////////////////////////////////////////


template <typename stringT>
inline std::vector<stringT>
split( const stringT &__str, const stringT &__sep, bool __skip_empty = false )
{
	std::vector<stringT> __ret;
	split( __ret, __str, __sep, __skip_empty );
	return __ret;
}


////////////////////////////////////////


template <typename stringT>
std::vector<stringT>
split( const stringT &__str, const typename stringT::value_type *__sep,
	   bool __skip_empty = false )
{
	return split( __str, stringT( __sep ), __skip_empty );
}


////////////////////////////////////////


template <typename stringT>
inline void
split( std::vector<stringT> &__ret, const stringT &__str,
	   typename stringT::value_type __sep,
	   bool __skip_empty = false )
{
	__ret.clear();

	if ( __skip_empty )
	{
		typename stringT::size_type __last = __str.find_first_not_of( __sep, 0 );
		typename stringT::size_type __cur = __str.find_first_of( __sep, __last );

		while ( __cur != stringT::npos || __last < __str.size() )
		{
			if ( __cur != __last )
				__ret.push_back( __str.substr( __last, __cur - __last ) );
			__last = __str.find_first_not_of( __sep, __cur );
			__cur = __str.find_first_of( __sep, __last );
		}
	}
	else
	{
		typename stringT::size_type __last = 0;
		typename stringT::size_type __cur = __str.find_first_of( __sep, __last );

		do
		{
			if ( __cur == stringT::npos )
			{
				if ( __last == __str.size() )
					__ret.push_back( stringT() );
				else
					__ret.push_back( __str.substr( __last ) );
				break;
			}
			else
				__ret.push_back( __str.substr( __last, __cur - __last ) );

			__last = __cur + 1;
			__cur = __str.find_first_of( __sep, __last );
		} while ( __last != stringT::npos );
	}
}


////////////////////////////////////////


template <typename stringT>
std::vector<stringT>
split( const stringT &__str, typename stringT::value_type __sep,
	   bool __skip_empty = false )
{
	std::vector<stringT> __ret;
	split( __ret, __str, __sep, __skip_empty );
	return __ret;
}


////////////////////////////////////////


} // namespace base



