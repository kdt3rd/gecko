//
// Copyright (c) 2014 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
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
		lang = n;
	}

	for ( char &x: lang )
		x = static_cast<char>( std::tolower( x ) );

	return lang.substr( 0, lang.find_first_of( "-_@." ) );
}

////////////////////////////////////////

}

}
