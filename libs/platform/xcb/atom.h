//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once

#include <xcb/xcb.h>
#include <string>

namespace platform { namespace xcb
{

////////////////////////////////////////

std::string get_atom_name( xcb_connection_t *c, xcb_atom_t a );
xcb_atom_t get_atom( xcb_connection_t *c, const std::string &name );

////////////////////////////////////////

} }

