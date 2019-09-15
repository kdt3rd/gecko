// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "platform.h"

#include <platform/mswin/system.h>
//#include <platform/dummy/system.h>

namespace platform
{
////////////////////////////////////////

std::vector<platform> &platform::init( void )
{
    static std::vector<platform> plat{
        platform(
            "mswin",
            "gl",
            []( const std::string &d ) {
                return std::make_shared<mswin::system>( d );
            } )
        //	platform::platform::enroll( "dummy", "dummy", [] { return std::make_shared<dummy::system>(); } );
    };
    return plat;
}

} // namespace platform
