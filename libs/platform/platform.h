// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "system.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace platform
{
////////////////////////////////////////

class platform
{
public:
    const std::string &name( void ) const { return _name; }
    const std::string &renderer( void ) const { return _render; }

    std::shared_ptr<system>
    create( const std::string &disp = std::string() ) const
    {
        return _creator( disp );
    }

    /// This returns a list of all platforms enabled
    static const std::vector<platform> &list( void );
    /// This will return the first platform in the list of
    /// supported / compiled-in platforms as the 'preferred'
    /// TODO: add a configuration file about which one to use?
    static const platform &preferred( void );
    /// This attempts to find and create a system for a running
    /// platform - meant as a mechanism to dynamically discover
    /// whether a scenario like X is running or wayland...
    static std::shared_ptr<system>
    find_running( const std::string &disp = std::string() );

    platform( void )             = default;
    platform( const platform & ) = default;
    platform &operator=( const platform & ) = default;
    platform( platform && )                 = default;
    platform &operator=( platform && ) = default;
    ~platform( void )                  = default;

private:
    // each O.S. need to implement this
    static std::vector<platform> &init( void );

    platform(
        std::string name,
        std::string render,
        const std::function<std::shared_ptr<system>( const std::string & )>
            &creator );

    std::string                                                   _name;
    std::string                                                   _render;
    std::function<std::shared_ptr<system>( const std::string & )> _creator;
};

////////////////////////////////////////

} // namespace platform
