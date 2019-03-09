// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

////////////////////////////////////////

namespace scene
{

///
/// @brief Class backend provides the virtual base for a system that will consume a scene.
///
/// There will probably be intermediate virtual layers that build
/// additional functionality, such as a renderer, display engine, or
/// similar. However, this base exists as the core mechanism by which
/// changes to the scene tree / graph topologies are notified to
/// interested consumers of the scene.
///
class backend
{
public:
	virtual ~backend( void );

    virtual void notify_changed( const node_reference &nr, uint32_t changeKind ) = 0;
    virtual void notify_created( const node_reference &nr ) = 0;
    virtual void notify_destroyed( const node_reference &nr ) = 0;

private:

};

} // namespace scene



