// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "web_base.h"

#include <base/uri.h>
#include <map>
#include <net/tcp_socket.h>

namespace web
{
////////////////////////////////////////

/// @brief HTTP request
class request : public web_base
{
public:
    /// @brief Server constructor
    /// The request is read from the (client) socket.
    request( net::tcp_socket &socket );

    /// @brief Client constructor
    /// Construct a request to send to the server.
    request(
        std::string      method,
        const base::uri &path,
        std::string      version = "1.1" );

    virtual ~request( void );
    request( const request & ) = default;
    request( request && )      = default;
    request &operator=( const request & ) = default;
    request &operator=( request && ) = default;

    /// @brief HTTP method
    /// The method (e.g GET, PUT, or POST).
    const std::string &method( void ) const { return _method; }

    /// @brief HTTP request URI
    const base::uri &path( void ) const { return _path; }

    /// @brief Get the full map of HTTP header.
    const std::map<std::string, std::string> &header( void ) const
    {
        return _header;
    }

    /// @brief Send the request to the give socket
    void send( net::tcp_socket &socket );

private:
    std::string _method;
    base::uri   _path;
};

////////////////////////////////////////

/// @brief Print (for debugging)
std::ostream &operator<<( std::ostream &out, const request &r );

////////////////////////////////////////

} // namespace web
