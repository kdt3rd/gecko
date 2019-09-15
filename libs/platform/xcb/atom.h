// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <string>
#include <xcb/xcb.h>

namespace platform
{
namespace xcb
{
////////////////////////////////////////

std::string get_atom_name( xcb_connection_t *c, xcb_atom_t a );
xcb_atom_t  get_atom( xcb_connection_t *c, const std::string &name );

////////////////////////////////////////

} // namespace xcb
} // namespace platform
