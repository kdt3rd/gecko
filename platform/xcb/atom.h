
#pragma once

#include <xcb/xcb.h>
#include <core/pointer.h>
#include <core/contract.h>

namespace xcb
{

////////////////////////////////////////

std::string get_atom_name( xcb_connection_t *c, xcb_atom_t a )
{
	auto cookie = xcb_get_atom_name_unchecked( c, a );
	auto reply = core::wrap_cptr( xcb_get_atom_name_reply( c, cookie, NULL ) );

	precondition( bool(reply), "error getting atom name" );

	size_t len = xcb_get_atom_name_name_length( reply.get() );
	return std::string( xcb_get_atom_name_name( reply.get() ), len );
}

////////////////////////////////////////

}

