// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "evaluation_context.h"
#include "time.h"

#include <base/fast_mutex.h>

////////////////////////////////////////

namespace scene
{
class node
{
public:
    struct flags
    {
        static constexpr uint32_t kInheritsValues = 0x1;
    };
    /// will receive a default object id for later addressing
    node( const char *n );
    node( const char *n, uint64_t override_id );
    ~node( void );

    uint64_t    id( void ) const { return _id; }
    const char *name( void ) const { return _name; }

    /// assumes the names of the levels are separated by '/'
    node *find( const char *n ) const;

    /// marks the node as out-of-date (needs re-evaluation) from
    /// begin to end (inclusive)
    void invalidate( const time &begin, const time &end );

    void             add_dependency( const node &n );
    transform_stack &get_transform_stack( const time &ctxt );
    matrix &         model_to_world( const time &ctxt ) const;

    void process( const evaluation_context &ctxt );

protected:
    inline void read_lock( void ) { _rw_mutex.read_lock(); }
    inline void read_unlock( void ) { _rw_mutex.read_unlock(); }
    inline void write_lock( void ) { _rw_mutex.lock(); }
    inline void write_unlock( void ) { _rw_mutex.unlock(); }

    inline void take_read_lock private
        :
        // hrm, could trim this down some, but 64 bytes gives 16384 / megabyte and
        // lines up well w/ L1 cache, etc.

        static_assert(
            sizeof( base::fast_rw_mutex ) == 4,
            "expect 4 bytes for fast rw mutex" );
    base::fast_rw_mutex _rw_mutex = {}; // 4

    uint32_t _flags_and_stuff = 0; // 4

    uint64_t        _id       = uint64_t( -1 ); // 8 -> 16
    const char *    _name     = nullptr;        // 8 -> 24
    node *          _parent   = nullptr;        // 8 -> 32
    implementation *_impl     = nullptr;        // 8 -> 40
    values *        _values   = nullptr;        // 8 -> 48
    children *      _children = nullptr;        // 8 -> 56
    connections *   _conns    = nullptr;        // 8 -> 64
};

} // namespace scene
