// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "time_util.h"

#include "contract.h"

#include <iomanip>
#include <sstream>
#include <time.h>

////////////////////////////////////////

namespace base
{
////////////////////////////////////////

std::string as_ISO8601( std::chrono::system_clock::time_point tp )
{
    auto stime = std::chrono::system_clock::to_time_t( tp );

    // TODO: locale?
    std::stringstream sbuf;

    // don't use std::localtime as it is not thread safe
    // according to docs, need to call tzset first to make sure it's initialized
    // when using _r functions

    struct tm ltime;
#ifdef _WIN32
    _tzset();
    struct tm *ltimep = &ltime;
    errno_t    e      = localtime_s( ltimep, &stime );
    if ( e != 0 )
        throw_errno( "Unable to convert to local time" );
#else
    tzset();
    struct tm *ltimep = localtime_r( &stime, &ltime );
#endif

    sbuf << std::put_time( ltimep, "%FT%T%z" );
    return sbuf.str();
}

////////////////////////////////////////

int64_t local_offset_to_UTC( void )
{
    auto stime = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now() );

    // don't use std::localtime as it is not thread safe
    // according to docs, need to call tzset first to make sure it's initialized
    // when using _r functions
    tzset();

    struct tm ltime, gtime;
#ifdef _WIN32
    errno_t e = gmtime_s( &ltime, &stime );
    if ( e != 0 )
        throw_errno( "Unable to convert to GMT" );
    e = localtime_s( &ltime, &stime );
    if ( e != 0 )
        throw_errno( "Unable to convert to local time" );
#else
    gmtime_r( &stime, &gtime );
    localtime_r( &stime, &ltime );
#endif
    int64_t diff =
        ( ( ( ltime.tm_hour - gtime.tm_hour ) * 60L +
            ( ltime.tm_min - gtime.tm_min ) ) *
              60L +
          ( ltime.tm_sec - gtime.tm_sec ) );
    int daydelt = ltime.tm_mday - gtime.tm_mday;
    if ( daydelt == 1 || ( daydelt < -1 ) )
        diff += 24L * 60 * 60;
    else if ( daydelt == -1 || ( daydelt > 1 ) )
        diff -= 24L * 60 * 60;

    return diff;
}

} // namespace base
