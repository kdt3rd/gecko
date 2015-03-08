//
// Copyright (c) 2014 Kimball Thurston
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

#include "language.h"
#include <locale>
#include <cstdlib>
#include <cctype>


////////////////////////////////////////


namespace base
{
namespace locale
{

////////////////////////////////////////

void set( const std::string &l )
{
	if ( l.empty() )
		std::locale::global( std::locale::classic() );

	std::locale loc( l.c_str() );
	std::locale::global( loc );
}

////////////////////////////////////////

std::string language( void )
{
	std::string n = std::locale("").name();
	if ( n == "*" || n == "C" )
		return std::string();

	if ( n.empty() )
	{
		const char *envP = std::getenv( "LC_CTYPE" );
		if ( envP && envP[0] )
			return std::string( envP );
		envP = std::getenv( "LC_ALL" );
		if ( envP && envP[0] )
			return std::string( envP );
		envP = std::getenv( "LANG" );
		if ( envP && envP[0] )
			return std::string( envP );

		return std::string();
	}

	std::string::size_type nPos = n.find( "LC_NAME=" );
	std::string::size_type ePos = std::string::npos;
	std::string lang;
	if ( nPos != std::string::npos )
	{
		ePos = n.find_first_of( ';', nPos + 8 );
		if ( ePos != std::string::npos )
			lang = n.substr( nPos + 8, ePos - ( nPos + 8 ) );
		else
			lang = n.substr( nPos + 8 );
	}
	else
	{
		lang = nPos;
	}

	for ( char &x: lang )
		x = std::tolower( x );

	return lang.substr( 0, lang.find_first_of( "-_@." ) );
}

////////////////////////////////////////

}

}
