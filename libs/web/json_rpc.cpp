//
// Copyright (c) 2015 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "json_rpc.h"
#include <iostream>

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
	try
	{
		tmp.parse( rpc );
	}
	catch ( std::exception &e )
	{
		return error( -32700, base::json( e.what() ) );
	}

	return local_call( tmp );
}

////////////////////////////////////////

base::json json_rpc::local_call( const base::json &rpc )
{
	try
	{
		if ( !rpc.has( "jsonrpc" ) )
			return error( -32600, "jsonrpc version missing" );
		if( rpc["jsonrpc"].get<base::json_string>() != "2.0" )
		{
			std::cout << "ACK: " << rpc << std::endl;
			return error( -32600, base::json( std::string( base::format( "expect jsonrpc version 2.0, got {0}", rpc["jsonrpc"].get<std::string>() ) ) ) );
		}
	}
	catch ( std::exception &e )
	{
		return error( -32600, e.what() );
	}

	std::string method;
	try
	{
		if ( !rpc.has( "method" ) )
			return error( -32601, "no method specified" );
		method = rpc["method"].get<std::string>();
		if ( method.empty() )
			return error( -32601, "empty method name" );
	}
	catch ( std::exception &e )
	{
		return error( -32601, e.what() );
	}

	try
	{
		base::json result;
		result["jsonrpc"] = "2.0";
		if ( rpc.has( "id" ) )
			result["id"] = rpc.at( "id" );
		if ( rpc.has( "params" ) )
			result.append( call( method.c_str(), rpc["params"] ) );
		else
			result.append( call( method.c_str(), base::json() ) );
		return result;
	}
	catch ( std::exception &e )
	{
		return error( -32603, e.what() );
	}
}

////////////////////////////////////////

base::json json_rpc::call( const char *name, const base::json &param )
{
	try
	{
		base::json result;
		base::json &out = result["result"];
		_function.at( name )->call( out, param );
		return result;
	}
	catch ( std::exception &e )
	{
		return error( -32603, base::json( e.what() ) );
	}
}

////////////////////////////////////////

base::json json_rpc::error( int code, base::json &&data )
{
	base::json result;
	base::json &error = result["error"];
	error["code"] = code;
	switch ( code )
	{
		case -32700:
			error["message"] = "Parse error";
			break;

		case -32600:
			error["message"] = "Invalid request";
			break;

		case -32601:
			error["message"] = "Method not found";
			break;

		case -32602:
			error["message"] = "Invalid params";
			break;

		case -32603:
			error["message"] = "Internal error";
			break;

		default:
			error["message"] = "Server error";
			break;
	}
	if ( data.valid() )
		error["data"] = std::move( data );

	return result;
}

////////////////////////////////////////

void json_rpc::throw_error( const base::json &r )
{
	if ( r.has( "error" ) )
	{
		auto &err = r["error"];
		if ( err.has( "data" ) )
		{
			auto &data = err["data"];
			if ( data.is<std::string>() )
				throw_runtime( data.get<std::string>() );
			else
			{
				std::string tmp;
				tmp << data;
				throw_runtime( tmp );
			}
		}
		else if ( err.has( "message" ) )
		{
			auto &data = err["message"];
			if ( data.is<std::string>() )
				throw_runtime( data.get<std::string>() );
			else
			{
				std::string tmp;
				tmp << data;
				throw_runtime( tmp );
			}
		}
	}
	else if ( !r.has( "result" ) )
		throw_runtime( "json rpc expected error or result" );
}

////////////////////////////////////////

}
