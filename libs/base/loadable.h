// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include "const_string.h"

#include <memory>

////////////////////////////////////////

namespace base
{
///
/// @brief Class loadable provides a wrapper around a dynamically
/// loaded executable file (i.e. plugin or other dso).
///
/// Linux, Solaris, et al. provide an isolation mechanism
/// (dlmopen). However, windows does not seem to provide such a
/// mechanism via the generic API. However, linking has to be
/// hard-bound under windows, so in practice this is not the same
/// issue.
///
/// There is a limited amount of isolation slots available to dlmopen (16 under glibc), so rather than a generic flag
///
class loadable
{
public:
    enum class isolation
    {
        NORMAL,
        GLOBALIZE,
        ISOLATE
    };

    loadable( std::string lpath, isolation iso = isolation::NORMAL );
    loadable( void )             = default;
    loadable( const loadable & ) = default;
    loadable( loadable && )      = default;
    loadable &operator=( const loadable & ) = default;
    loadable &operator=( loadable && ) = default;
    ~loadable( void );

    void *
    symbol( const cstring &symName, const cstring &symVersion = cstring() );

    static void *find_app_symbol(
        const cstring &symName, const cstring &symVersion = cstring() );
    static void *find_next_symbol(
        const cstring &symName, const cstring &symVersion = cstring() );

private:
    std::string           _loadpath;
    std::shared_ptr<void> _dso;
};

} // namespace base
