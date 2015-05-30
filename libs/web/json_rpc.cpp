
#include "json_rpc.h"

namespace web
{

////////////////////////////////////////

namespace detail
{

base_function::~base_function( void )
{
}

}

////////////////////////////////////////

json_rpc::json_rpc( void )
{
}

////////////////////////////////////////

base::json json_rpc::local_call( const std::string &rpc )
{
	base::json tmp;
	tmp.parse( rpc );
	return local_call( tmp );
}

////////////////////////////////////////

base::json json_rpc::local_call( const base::json &rpc )
{
	precondition( rpc["jsonrpc"].get<std::string>() == "2.0", "server only allows json RPC 2.0" );

	std::string method = rpc["method"].get<std::string>();
	if ( method.empty() )
		throw_runtime( "json RPC requires non-empty method name" );

	base::json result;
	result["jsonrpc"] = "2.0";
	result["id"] = rpc.at( "id" );
	if ( rpc.has( "params" ) )
		result["result"] = call( method.c_str(), rpc["params"] );
	else
	{
		base::json params;
		result["result"] = call( method.c_str(), params );
	}
	return std::move( result );
}

////////////////////////////////////////

base::json json_rpc::call( const char *name, const base::json &param )
{
	try
	{
		base::json result;
		_function.at( name )->call( result, param );
		return std::move( result );
	}
	catch ( ... )
	{
		throw_add( "json RPC call {0}( {1} ) error", name, param );
	}
}

////////////////////////////////////////

}
