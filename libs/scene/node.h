// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "evaluation_context.h"
#include "time.h"

#include <base/concurrent_vector.h>
#include <base/string_pool.h>

#include <atomic>
#include <string>
#include <initializer_list>
#include <vector>

////////////////////////////////////////

namespace scene
{
class implementation;

/// for keeping track of inter-node dependencies. This could be:
/// - instance / exemplar link
/// - expression references
/// - compositing style noodle-style input connections
/// Does this deserve to be on each node? How many nodes will
/// have references? (vs just having a map of connections in the scene)
/// these bypass hierarchy, so tracking readers / writers might be
/// harder?
class connections;

/// @brief The core of the scene elements
///
/// So the idea is that this needs to be as lock / contention free
/// as possible. Borrowing from other lock free ideas, it seems like
/// if you have a tree:
/// root
///  + A
///  | + C
///  | + D
///  |
///  + B
///  | + E
///  | | + F
///  | | + G
///  | |
///  | + H
///  + J
///
/// and you want to reparent E from B to J, you can atomic swap pointers.
/// and be good to go. sort of. If there is another thread reading attrs
/// while you do that swap, this can be fine, as long as the appropriate
/// dirty state is set. (although doing that in a consistent state
/// might be tricky?)
///
/// If you want to delete E and reparent F & G to B, then you have to wait
/// for all threads who are reading values B or below to coalesce and
/// finish prior to actually deleting E
///
/// If you want to change a value of an attribute in E, then that update
/// can not conflict with any readers at E or below looking for attributes
/// (assuming inherited attributes)
///
/// Adding a child depends on being able to modify the list safely. This can
/// affect iterators traversing the node hierarchy when the change is made,
/// although as long as they are coded to just behave as iterators, then
/// this does not seem like an issue (modulo the dirty state and knowing they
/// may occasionally not see a new node as it is inserted)
///
class node
{
public:
    using string_type      = base::size_tagged_string;
    using children_storage = base::concurrent_vector<node *>;
    using data_storage     = std::shared_ptr<void>;
    using flag_type        = uint64_t;

    /// @brief collection of flags for a node
    ///
    /// There are 64 flags possible
    struct flags
    {
        static constexpr flag_type kInheritsValues = 0x1;
    };

    node( void ) = default;
    explicit node( const string_type &n );
    ~node( void );

    node( const node & ) = delete;
    node &operator=( const node & ) = delete;

    node( node && ) noexcept;
    node &operator=( node && ) noexcept;

    string_type name( void ) const { return _name; }
    void        path( std::string &p ) const;
    void        relative_path( const node *parent, std::string &p ) const;

    /// assumes the names of the levels are separated by '/'
    node *      find( const char *n );
    const node *find( const char *n ) const;

    flag_type flags( void ) const
    {
        return _flags.load( std::memory_order_relaxed );
    }
    void set_flag( flag_type f ) { _flags |= f; }
    void clear_flag( flag_type f ) { _flags &= ~f; }

    /// marks the node as out-of-date (needs re-evaluation) from
    /// startTime to endTime (inclusive)
    void invalidate( const time &startTime, const time &endTime );

    void evaluate( const evaluation_context &ctxt );
    void expand( const evaluation_context &ctxt );

    void add_child( node *child );
    void add_children( std::initializer_list<node *> l );
    void add_children( const std::vector<node *> &l );
    void remove_child( node *child );

    node *                parent( void ) const { return _parent; }
    implementation *      impl() { return _impl; }
    const implementation *impl() const { return _impl; }

    data_storage data() const;

    children_storage::iterator begin( void ) { return _children.begin(); }
    children_storage::iterator end( void ) { return _children.end(); }

    children_storage::const_iterator begin( void ) const
    {
        return _children.begin();
    }
    children_storage::const_iterator end( void ) const
    {
        return _children.end();
    }

    void swap( node &n );

protected:
private:
    const node *internal_find( const char *n, size_t nLeft ) const;

    void
    path_recurse( const node *parent, std::string &p, size_t reserve ) const;

    // tried to make this smaller, but seems like everything becomes
    // an indirect lookup, which doesn't seem worth it. If we start
    // storing instances as nodes, then fine, but it seems like
    // those are better as an "instance list" node with appropriate
    // arrays and such.
    // so leave this as 64 bytes, giving 16384 nodes / megabyte of
    // scene overhead, which should be quickly dwarfed by the
    // attributes and actual data. 64 bytes also lines up well with
    // x86 L1 cache, and if these are stored in a pool in the scene,
    // should be quite efficient.

    std::atomic<flag_type> _flags    = ATOMIC_VAR_INIT( 0 );
    string_type            _name     = {};
    node *                 _parent   = nullptr;
    implementation *       _impl     = nullptr;
    data_storage           _data     = {};
    children_storage       _children = {};
};
static_assert( sizeof( node ) == 64, "Expect a 64-byte node" );

inline void swap( node &a, node &b )
{
    a.swap( b );
}

} // namespace scene

namespace std
{
inline void swap( scene::node &a, scene::node &b )
{
    a.swap( b );
}
} // namespace std
