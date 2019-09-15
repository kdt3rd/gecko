// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <cstddef>
#include <cstdint>

////////////////////////////////////////

namespace scene
{
class node_reference;
class setting;

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

    virtual void notify_changed(
        const node_reference &nr,
        const time &          startTime,
        const time &          endTime,
        uint32_t              changeKind ) = 0;
    virtual void notify_created( const node_reference &nr ) = 0;

    /// Called when the node is *requested* to be deleted. If
    /// this returns false, the node will NOT be destroyed
    ///
    /// NB: since there may be multiple backends live, any of
    /// which can deny a deletion, do not assume the node will actually
    /// be destroyed, and instead pay attention to @sa notify_destroyed
    virtual bool request_delete( const node_reference &nr ) = 0;

    /// Called after a node is destroyed and removed from the scene
    virtual void notify_destroyed( uint64_t nid ) = 0;

    virtual size_t         setting_count( void ) const = 0;
    virtual setting &      setting( size_t idx )       = 0;
    virtual const setting &setting( size_t idx ) const = 0;

private:
};

} // namespace scene
