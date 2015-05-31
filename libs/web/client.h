
#pragma once

#include <string>
#include <base/uri.h>

#include "response.h"

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
	response post( const base::uri &host, std::string &&v, double timeout = 0.0 );

private:
	std::string _agent;
};

////////////////////////////////////////

}

