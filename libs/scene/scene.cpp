// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "scene.h"

////////////////////////////////////////

namespace scene
{

////////////////////////////////////////

node_reference scene::world( void ) const
{
    return node_reference( this, &_root );
}

} // namespace scene
