// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "response.h"

#include <base/json.h>
#include <base/uri.h>
#include <string>

namespace web
{
////////////////////////////////////////

/// @brief HTTP client
class client
{
public:
    /// @brief Constructor
    client( std::string agent = "HTTPClient/0.0" );

    /// @brief Get an HTTP resource.
    response get( const base::uri &host, double timeout = 0.0 );
    response
    post( const base::uri &host, std::string &&v, double timeout = 0.0 );
    response
    post( const base::uri &host, const base::json &v, double timeout = 0.0 )
    {
        std::string tmp;
        tmp << v;
        return post( host, std::move( tmp ), timeout );
    }

private:
    std::string _agent;
};

////////////////////////////////////////

} // namespace web
