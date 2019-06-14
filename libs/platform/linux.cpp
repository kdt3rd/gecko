// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include "platform.h"
#ifdef HAVE_XLIB
#    include <platform/xlib/system.h>
#endif
#ifdef HAVE_WAYLAND
#    include <platform/wayland/system.h>
#endif

#if !defined( HAVE_WAYLAND ) && !defined( HAVE_XLIB )
#    error                                                                     \
        "No valid platform libraries found, please install necessary libraries"
#endif

//#include <platform/xcb/system.h>
//#include <platform/dummy/system.h>
#include <base/compiler_support.h>
#include <clocale>
#include <mutex>

////////////////////////////////////////

namespace
{
static std::once_flag lin_init_flag;

static void initialize( void ) { std::setlocale( LC_ALL, "" ); }

} // namespace

namespace platform
{
////////////////////////////////////////

std::vector<platform> &platform::init( void )
{
    std::call_once( lin_init_flag, &initialize );

    static std::vector<platform> plat{
#ifdef HAVE_WAYLAND
        platform(
            "wayland",
            "egl",
            []( const std::string &d ) {
                return std::make_shared<wayland::system>( d );
            } ),
#endif
#ifdef HAVE_XLIB
        platform(
            "xlib",
            "gl",
            []( const std::string &d ) {
                return std::make_shared<xlib::system>( d );
            } ),
#endif
    };

    return plat;
}

} // namespace platform
