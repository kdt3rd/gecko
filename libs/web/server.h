
#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <string>

#include <base/thread_pool.h>
#include "response.h"
#include "request.h"

namespace web
{

////////////////////////////////////////

class server
{
public:
	typedef std::function<void( response&, request& )> handler;

	server( uint16_t port, size_t threads = 1 );

	handler &resource( const std::string &method, const std::string &re );

	handler &default_resource( const std::string &method );

	void run( void );

	static void not_found( response &resp, request &req );

private:
	void handle_client( net::tcp_socket &client );
	std::map<std::string,std::map<std::string,handler>> _resources;
	std::map<std::string,handler> _defaults;

	bool _done = false;
	uint16_t _port;
	size_t _threads = 0;
};

////////////////////////////////////////

}

