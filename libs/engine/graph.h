// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "node.h"
#include "registry.h"
#include "subgroup.h"
#include "types.h"

#include <atomic>
#include <base/const_string.h>
#include <mutex>
#include <ostream>
#include <set>
#include <typeinfo>
#include <vector>

////////////////////////////////////////

namespace engine
{
///
/// @brief Class graph provides storage for a graph of operations
///
/// The idea is to have this graph be dynamic in nature, and storing
/// what is to be done until a value is needed, at which point the
/// graph as it exists can be optimized based on what is known, and a
/// request processed, which may only be a sub-portion of the
/// graph. Then the graph may be continued to be edited, with the
/// previously processed (and no longer referenced) portions
/// discarded at some point.
///
/// How this happens in the end remains to be seen. For now, the above
/// behavior described is very explicit.
///
class graph
{
public:
    typedef std::vector<node>::iterator               iterator;
    typedef std::vector<node>::const_iterator         const_iterator;
    typedef std::vector<node>::reverse_iterator       reverse_iterator;
    typedef std::vector<node>::const_reverse_iterator const_reverse_iterator;

    /// The registry must exist for the lifetime of the graph
    explicit graph( const registry &o );
    ~graph( void );

    inline const registry &op_registry( void ) const;

    template <typename V> inline node_id add_constant( V &&v )
    {
        hash h;
        h << v;
        return add_node(
            _ops.find_constant( typeid( V ) ),
            any( std::move( v ) ),
            nulldim,
            {},
            h );
    }

    template <typename V> inline node_id add_constant( const V &v )
    {
        hash h;
        h << v;
        return add_node(
            _ops.find_constant( typeid( V ) ), any( v ), nulldim, {}, h );
    }

    node_id add_node(
        const base::cstring &          opname,
        const dimensions &             d,
        std::initializer_list<node_id> inputs );

    void tag_rvalue( node_id n );

    const any &get_value( node_id n );

    node_id copy_node( const graph &o, node_id n );
    node_id move_node( graph &o, node_id n );
    void    remove_node( node_id n );

    inline size_t      size( void ) const { return _nodes.size(); }
    inline node &      operator[]( node_id n ) { return _nodes[n]; }
    inline const node &operator[]( node_id n ) const { return _nodes[n]; }

    inline iterator       begin( void ) { return _nodes.begin(); }
    inline const_iterator begin( void ) const { return _nodes.begin(); }
    inline const_iterator cbegin( void ) const { return _nodes.cbegin(); }
    inline iterator       end( void ) { return _nodes.end(); }
    inline const_iterator end( void ) const { return _nodes.end(); }
    inline const_iterator cend( void ) const { return _nodes.cend(); }

    inline reverse_iterator       rbegin( void ) { return _nodes.rbegin(); }
    inline const_reverse_iterator rbegin( void ) const
    {
        return _nodes.rbegin();
    }
    inline const_reverse_iterator crbegin( void ) const
    {
        return _nodes.crbegin();
    }
    inline reverse_iterator       rend( void ) { return _nodes.rend(); }
    inline const_reverse_iterator rend( void ) const { return _nodes.rend(); }
    inline const_reverse_iterator crend( void ) const { return _nodes.crend(); }

    /// returns true if one of the (recursive) inputs of node n is one
    /// of the nodes in the vector
    bool has_ancestor( node_id n, const std::vector<node_id> &nodes ) const;
    /// returns true if a is an eventual ancestor of n
    bool has_ancestor( node_id n, node_id a ) const;

    void optimize( void );

    // remove
    void clean_graph( void );

    void dump_dot( const std::string &fn, bool incHash = false ) const;
    void dump_dot( std::ostream &os, bool incHash = false ) const;
    void dump_refs( std::ostream &os ) const;

    inline bool computing( void ) const { return _computing.load() > 0; }

private:
    graph( void )          = delete;
    graph( const graph & ) = delete;
    graph( graph && )      = delete;
    graph &operator=( const graph & ) = delete;
    graph &operator=( graph && ) = delete;

    const any &process( node_id nid );
    void       move_constants( void );
    void       apply_peephole( void );

    void    apply_grouping( void );
    void    clear_grouping( void );
    size_t  find_subgroup( node_id n ) const;
    size_t  merge_subgroups( size_t a, size_t b );
    void    split_subgroup( size_t i, node_id n );
    bool    ok_to_merge( size_t sg, size_t check, node_id n );
    void    rotate_node( node_id oldpos, node_id newpos );
    node_id find_node( const hash::value &hv );

    node_id add_node(
        op_id                          op,
        any                            value,
        const dimensions &             d,
        std::initializer_list<node_id> inputs );
    node_id add_node(
        op_id                          op,
        any                            value,
        const dimensions &             d,
        std::initializer_list<node_id> inputs,
        hash &                         h );
    node_id add_node(
        op_id                          op,
        any                            value,
        const dimensions &             d,
        std::initializer_list<node_id> inputs,
        const hash::value &            hv );
    node_id add_node(
        op_id                       op,
        any                         value,
        const dimensions &          d,
        const std::vector<node_id> &inputs,
        const hash::value &         hv );
    node_id add_node(
        op_id                       op,
        any                         value,
        const dimensions &          d,
        const std::vector<node_id> &inputs,
        hash &                      h );

    //	template <typename V> friend class computed_value;
    friend class computed_base;

    typedef void ( *rewrite_notify )( void *, node_id, node_id );

    void update_hash_map( void );
    void update_refs( const std::map<node_id, node_id> &newnodemap );
    void reference( node_id n, rewrite_notify notify, void *ud );
    void unreference( node_id n, rewrite_notify notify, void *ud ) noexcept;

    const registry &_ops;

    std::vector<node>                                      _nodes;
    std::set<node_id>                                      _process_list;
    typedef std::vector<std::pair<rewrite_notify, void *>> reference_list;
    std::map<node_id, reference_list>                      _ref_counts;
    std::map<hash::value, node_id>                         _hash_to_node;
    std::vector<subgroup>                                  _subgroups;
    std::map<node_id, size_t>                              _node_to_subgroup;
    node_id          _start_of_processing = 0;
    std::mutex       _value_get_mutex;
    std::atomic<int> _computing;
};

////////////////////////////////////////

inline const registry &graph::op_registry( void ) const { return _ops; }

} // namespace engine
