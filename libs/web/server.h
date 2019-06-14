// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include "request.h"
#include "response.h"

#include <base/thread_pool.h>
#include <cstdint>
#include <functional>
#include <map>
#include <string>

namespace web
{
////////////////////////////////////////

/// @brief A web server.
class server
{
public:
    /// @brief Type for a function handling an HTTP request
    typedef std::function<void( request &, net::tcp_socket &client )> handler;

    /// @brief Constructor
    server( uint16_t port, size_t threads = 1 );

    /// @brief Add a resource handler
    handler &resource( const std::string &method, const std::string &re );

    /// @brief Add a default resource handler
    handler &default_resource( const std::string &method );

    /// @brief Run the server (forever)
    void run( void );

    /// @brief Simple not found handler.
    static void not_found( request &req, net::tcp_socket &client );

private:
    void handle_client( net::tcp_socket &client );
    std::map<std::string, std::map<std::string, handler>> _resources;
    std::map<std::string, handler>                        _defaults;

    bool     _done = false;
    uint16_t _port;
    size_t   _threads = 0;
};

////////////////////////////////////////

} // namespace web
