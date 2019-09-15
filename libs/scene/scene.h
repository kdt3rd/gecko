// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "node_reference.h"

#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include <base/string_pool.h>


////////////////////////////////////////

namespace scene
{
class light_hierarchy;
class provider;
class backend;

/// @brief the top level interface into a scene
///
/// a scene should have representative entities for every component going
/// into the scene - the common database to store it all. Another key
/// element is that access to manipulate a scene should be fully
/// multi-thread safe, in that there should be able to be multiple threads
/// modifying the scene data at once for expansion / update, and while
/// the view might change from moment to moment, any view should be
/// fully consistent at the instant it is viewed.
///
/// Components in the database should be:
///
/// "Assets" for this scene
/// - Camera definitions (left, right, etc.)
/// - Renderable geometry / volumes / etc. with their relevant animation, layout, and material definitions
/// - Textures, shaders, etc. used for the above
/// - Imported images / audio / media
///    - metadata
///    - any pre-processing to prepare them (roto, denoise, keying, optical flow)
///    - the pre-processing may produce other images! (i.e. alpha channels, vectors, roto mattes)
///    - this results in concept of cached (pre-computed) images that could
///      be re-rendered if upstream stuff changes (todo: versioning?)
///    - want image "bundles" (or some better name) that encapsulate multi-part exr
///    - deep images
/// - Plug-in provided items
/// - Settings for procedurally generated bits (plugins)
/// - Other scenes (references vs. copies?)
///   - sub-components of this scene to make loading of individual work units faster?
///
/// "Manipulations" of the scene
/// - Rendering passes (groupings of geo, etc.), and relevant render settings
///    - any pre-processing
/// - Compositing - graph that has keying, etc. layering operations, and all the fun bits
///
/// Project Management related features?
/// TBD: These seem like the should be external, but there should be some sort of hook
/// to the scene so notes / todos can appear on a version of the scene...
/// - Appropriate version references of all of the above
/// - Notes (annotations on image, video of session)
/// - Tasks? pipeline?
///
/// The basic idea is as follows:
/// - there are input assets (geo, lights, cameras, image plates, volumes, etc.) [plugin type]
///   * Plug-in generators can appear here (procedural generators)
/// - you can define render passes to render geometry. This defines:
///   * A renderer, and the settings [plugin]
///     + output paths to cache the resulting image(s) [url scheme]
///     + additional AOVs
///   * A camera (or set of cameras)
///   * What geo, volumes, lights are in that render pass (or include all and then prune ala katana)
///   * Binds any custom material overrides
/// - Define compositions that prepare input image plates / media
///   * precomp stages
///   * these are just a specialization of a render pass, but add a bunch of
///     features that they are called out separately
/// - Define compositions that combine render passes and image plates / other compositions
///   * Plug-in generators can appear here (procedural generators)
///   * Use geo from layout for holdouts, etc.
///   * Deep integration
///   * compositions can be templates
///     - url scheme for template
///     - pattern system to bind render passes / plates to template inputs
///   * compositions can include external groups of nodes
///     - copy or reference (live update)
/// - Slap comp vs. final comp?
///
/// A tree structure view of the scene might be:
/// root
/// + world
/// |  + media captures (these should have a position in the world)
/// |     + plates (These are associated w/ a camera)
/// |     + audio
/// |     + lidar scans
/// |     + reference photos
/// |     + IBLs
/// |     + ...
/// |  + cgi elements (tbd: remove this layer?)
/// |     + geo
/// |     + volumes (special case of geo?)
/// |     + procedurals (special case of geo?)
/// |     + lights (differentiate environment/ibl from others?)
/// |        + what about emissive geo?
/// |     + simulations
/// |     + texture atlas
/// |     + materials
/// |  + cameras
/// |     + stereo rig
/// |  + ...
/// + production
///    + color management
///    + render passes
///       + uses a specific camera
///       + denoise (just a generic image proc / plate prep?)
///         - might want custom AOVs
///       + specifies an output path
///       + specifies list of cg elements, and their camera visibility
///    + compositions
///       + specifies an output path
///    + roto
///    + plate prep
///       + camera solves (produces a camera)
///       + optical flow
///          + specifies an output path for cache
///       + depth extract
///          + specifies an output path for cache
///       + keys
///    + paint
///       + storage location for strokes / image cache?
///
/// notice that the above are separated into two trees, one that is
/// just assets / items and another that is more how to process or
/// manipulate the above assets into a scene, and starts to hint at
/// a pipeline, although the above tree does not represent modeling,
/// simulations, or material definition currently other than as inputs.
/// However, the processing depends on the items, and changes to the
/// items need to properly invalidate the appropriate processing
/// trees. The production tree also has outputs and things it is
/// generating that each one may be inter-dependent upon, however, the
/// idea is that all of that can be re-generated from the scene data
/// and nodes / input recorded in the scene.
///
/// They are all placed into the same structure, such that they are
/// all nodes with values, which allows them to share the same
/// dependency tracking and notification system. Whether this is
/// viable depends on appropriate references to scale to larger
/// facilities.
///
/// Further, it would be expected that most of the items above would
/// actually be references to sub-files such that loading the top level
/// would be super fast, and you would only expand what you need
///
class scene
{
public:
    node_reference world( void ) const;
    /// \defgroup shortcuts into objects that exist in the world
    ///
    /// All of these items should have a position, even if it is
    /// relative to the camera implicitly
    ///
    /// @{
    node_reference media( void ) const;
    node_reference geo( void ) const;
    node_reference lights( void ) const;
    node_reference cameras( void ) const;
    /// object is a logical, renderable item:
    ///  - geo bound to a material
    node_reference objects( void ) const;
    /// @}

    /// \defgroup shortcuts into the production side
    /// @{
    node_reference color_management( void ) const;
    node_reference materials( void ) const;
    node_reference passes( void ) const;
    node_reference compositions( void ) const;
    /// @}

    /// for in-built providers, such that a plugin can load and override them
    void register_builtin_provider( const char *n, provider &p );
    void register_provider( const char *n, provider &p );
    void unregister_provider( const char *n );
    void set_lazy_provider_lookup( std::function<provider( const char *n )> f );
    const std::vector<provider> &providers( void ) const;

    void register_backend( const std::shared_ptr<backend> &b );
    void unregister_backend( const std::shared_ptr<backend> &b );
    const std::vector<std::shared_ptr<backend>> &backends( void ) const;

    /// assumes the names of the levels are separated by '/'
    node_reference find( uint64_t id ) const;
    node_reference find( const char *name ) const
    {
        // TODO: validate this is thread safe
        return node_reference( nullptr, _root.find( name ) );
    }

    /// This is equivalent to calling
    ///   find_or_create( root(), name, provider, type, version )
    ///
    /// @sa find_or_create
    inline node_reference find_or_create(
        const char *name,
        const char *provider,
        const char *type,
        const char *version )
    {
        node_reference tmproot( this, &_root );
        return find_or_create(
            tmproot.make_writable(), name, provider, type, version );
    }

    /// provider might be a name of a plugin
    ///
    /// type is the type of node to create
    ///
    /// version is the version of the object last saved (may be NULL
    /// to get latest)
    ///
    /// subname is the location name to create underneath parent. This
    /// can still be a tree path (a/b/c), which is considered relative
    /// to the parent.  if any intermediate levels do not exist, this
    /// will fail
    ///
    /// This is safe to do regardless of whether the provider / type /
    /// version actually exists. This allows a script to be loaded in
    /// a context where a plugin may not correctly exist
    node_reference find_or_create(
        const writable_node_ref &parent,
        const char *             subname,
        const char *             provider,
        const char *             type,
        const char *             version );

    void reparent( const node_reference &n, const node_reference &newparent );
    void remove( const node_reference &r, bool reparent_children );

    void add_connection(
        const node_reference &dependent,
        const node_reference &user,
        void *                item );
    void remove_connection(
        const node_reference &dependent,
        const node_reference &user,
        void *                item );

private:
    node _root;
    base::string_pool _name_pool;
};

} // namespace scene
