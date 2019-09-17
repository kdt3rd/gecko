// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "language.h"

#include <cctype>
#include <cstdlib>
#include <locale>

#ifdef _WIN32
#    include <algorithm>
#    include <iostream>
#    include <string>
#    include <vector>
#    include <windows.h>
#    include <winnls.h>

namespace
{
std::vector<std::wstring> locales;

// _configthreadlocale(_ENABLE_PER_THREAD_LOCALE);
// setlocale( LC_ALL, "en-US" );
BOOL CALLBACK LocalIterFunc( LPWSTR pStr, DWORD dwFlags, LPARAM lparam )
{
    locales.push_back( pStr );
    return TRUE;
}

void extractLocales()
{
    EnumSystemLocalesEx( &LocalIterFunc, 0, 0, NULL );

    for ( const auto &l: locales )
        std::wcout << l << std::endl;
}

} // namespace
#endif

////////////////////////////////////////

namespace base
{
namespace locale
{
////////////////////////////////////////

void set( const std::string &l )
{
#ifdef __MINGW32__
#else
    if ( l.empty() )
        std::locale::global( std::locale::classic() );

    // TODO: translate posix notation (en_US.UTF-8) to windows
    // English_UnitedStates or en-US?
    std::locale loc( l.c_str() );
    std::locale::global( loc );
#endif
}

////////////////////////////////////////

std::string language( void )
{
    std::string n = std::locale( "" ).name();
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
    std::string            lang;
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

std::locale query( const std::string &l )
{
#ifdef _WIN32
#    ifdef __MINGW32__
    // mingw only seems to have the "C" locale compiled in...
    return std::locale();
#    else
    // TODO: translate posix notation (en_US.UTF-8) to windows
    // English_UnitedStates or en-US?
    return std::locale( l );
#    endif
#else
    return std::locale( l );
#endif
}

////////////////////////////////////////

} // namespace locale

} // namespace base
