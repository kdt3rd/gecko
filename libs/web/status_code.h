//
// Copyright (c) 2015 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

namespace web
{

////////////////////////////////////////

enum class status_code
{
	CONTINUE = 100,
	PROTOCOL_SWITCH = 101,
	OK = 200,
	CREATED = 201,
	ACCEPTED = 202,
	NON_AUTH_INFO = 203,
	NO_CONTENT = 204,
	RESET_CONTENT = 205,
	PARTIAL_CONTENT = 206,
	MULTIPLE_CHOICE = 300,
	MOVED = 301,
	FOUND = 302,
	SEE_OTHER = 303,
	NOT_MODIFIED = 304,
	USE_PROXY = 305,
	TEMP_REDIRECT = 307,
	BAD_REQUEST = 400,
	UNAUTHORIZED = 401,
	PAYMENT_REQUIRED = 402,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	NOT_ACCEPTABLE = 406,
	PROXY_AUTH_REQUIRED = 407,
	REQUEST_TIMEOUT = 408,
	CONFLICT = 409,
	GONE = 410,
	LENGTH_REQUIRED = 411,
	PRECONDITION_FAILED = 412,
	INTERNAL_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	BAD_GATEWAY = 502,
	SERVICE_UNAVAILABLE = 503,
	GATEWAY_TIMEOUT = 504,
	VERSION_UNSUPPORTED = 505
};

////////////////////////////////////////

const char *reason_phrase( int code );

inline const char *reason_phrase( status_code s )
{
	return reason_phrase( static_cast<int>( s ) );
}

////////////////////////////////////////

}

