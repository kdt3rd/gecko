//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <xcb/xcb.h>
#include <stdexcept>
#include <core/contract.h>

namespace platform { namespace xcb
{

////////////////////////////////////////

template<typename Cookie, typename Reply, bool Checked, Reply *ReplyFunc( xcb_connection_t *, Cookie, xcb_generic_error_t ** )>
class reply_ptr
{
public:
	reply_ptr( void )
	{
		_connection = nullptr;
		_reply = nullptr;
	}

	reply_ptr( xcb_connection_t *c, const Cookie &cookie )
	{
		_connection = c;
		_cookie = cookie;
		precondition( _connection != nullptr, "null connection" );
	}

	~reply_ptr( void )
	{
		if ( _connection == nullptr && _reply != nullptr )
			free( _reply );
	}

	void reset( void )
	{
		if ( _connection == nullptr && _reply != nullptr )
			free( _reply );
		_connection = nullptr;
		_reply = nullptr;
	}

	void reset( xcb_connection_t *c, const Cookie &cookie )
	{
		if ( _connection == nullptr && _reply != nullptr )
			free( _reply );

		_connection = c;
		_cookie = cookie;
		precondition( _connection != nullptr, "null connection" );
	}

	Reply *get( void )
	{
		Reply *r = fetch();
		if ( r == nullptr )
			throw std::runtime_error( "null reply" );
		return r;
	}
	Reply *operator->( void )
	{
		return get();
	}

	Reply &operator*( void )
	{
		return *get();
	}

	operator bool()
	{
		return fetch() != nullptr;
	}

private:
	Reply *fetch( void )
	{
		if ( _connection )
		{
			xcb_connection_t *conn = _connection;
			_connection = nullptr;

			xcb_generic_error_t *err = nullptr;

			_reply = ReplyFunc( conn, _cookie, Checked ? &err : nullptr );
			if ( _reply == nullptr && xcb_connection_has_error( conn ) )
				throw std::runtime_error( "error with connection" );

			if ( err )
			{
				free( err );
				throw std::runtime_error( "reply error" );
			}
		}
		return _reply;
	}

	xcb_connection_t *_connection;
	union
	{
		Cookie _cookie;
		Reply *_reply;
	};
};

////////////////////////////////////////

typedef reply_ptr<xcb_get_atom_name_cookie_t, xcb_get_atom_name_reply_t, true, xcb_get_atom_name_reply> xcbpp_get_atom_name_reply_t;

xcbpp_get_atom_name_reply_t
xcbpp_get_atom_name( xcb_connection_t *c, xcb_atom_t a )
{
	auto cookie = xcb_get_atom_name( c, a );
	return xcbpp_get_atom_name_reply_t( c, cookie );
}

////////////////////////////////////////

typedef reply_ptr<xcb_intern_atom_cookie_t, xcb_intern_atom_reply_t, true, xcb_intern_atom_reply> xcbpp_intern_atom_reply_t;

xcbpp_intern_atom_reply_t
xcbpp_intern_atom( xcb_connection_t *c, uint8_t only_if_exist, uint16_t len, const char *name )
{
	auto cookie = xcb_intern_atom( c, only_if_exist, len, name );
	return xcbpp_intern_atom_reply_t( c, cookie );
}

////////////////////////////////////////

} }
