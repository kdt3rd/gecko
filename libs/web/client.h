
#pragma once

#include <string>
#include <base/uri.h>

#include "response.h"

namespace web
{

////////////////////////////////////////

class client
{
public:
	client( std::string agent );

	response get( const base::uri &host, double timeout = 0.0 );

private:
	std::string _agent;
};

////////////////////////////////////////

}

