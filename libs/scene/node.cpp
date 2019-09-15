// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "node.h"

#include <cstring>
#include <mutex>

#include <base/utility.h>

////////////////////////////////////////

namespace scene
{

////////////////////////////////////////

node::node( const string_type &n )
    : _flags( 0 )
    , _name( n )
{
}

////////////////////////////////////////

node::~node( void )
{
}

////////////////////////////////////////

node::node( node &&o ) noexcept
    : _flags( o._flags.load() )
    , _name( base::exchange( o._name, string_type( nullptr ) ) )
    , _parent( o._parent )
    , _impl( base::exchange( o._impl, nullptr ) )
    , _data( std::move( o._data ) )
    , _children( std::move( o._children ) )
{}

////////////////////////////////////////

node &node::operator=( node &&o ) noexcept
{
    if ( &o != this )
        swap( o );
    return *this;
}

////////////////////////////////////////

void node::swap( node &n )
{
    uint64_t f = _flags.load( std::memory_order_relaxed );
    _flags.store( n._flags.load( std::memory_order_relaxed ), std::memory_order_relaxed );
    n._flags.store( f, std::memory_order_relaxed );
    std::swap( _name, n._name );
    std::swap( _parent, n._parent );
    std::swap( _impl, n._impl );
    std::swap( _data, n._data );
    std::swap( _children, n._children );
}

////////////////////////////////////////

void node::path( std::string &p ) const
{
    path_recurse( nullptr, p, 0 );
}

////////////////////////////////////////

void node::relative_path( const node *par, std::string &p ) const
{
    path_recurse( par, p, 0 );
}

////////////////////////////////////////

void node::path_recurse( const node *parent, std::string &p, size_t reserve ) const
{
    // the root node is considered fully virtual (i.e. no real name)
    if ( _parent )
    {
        size_t nlen = _name.size();
        if ( _parent != parent )
            _parent->path_recurse( parent, p, reserve + nlen + 1 );
        p.push_back( '/' );
        p.append( _name.c_str(), nlen );
    }
    else
        p.reserve( reserve + 1 );
}

////////////////////////////////////////

node *node::find( const char *n )
{
    if ( n == nullptr || n[0] == '\0' )
        return nullptr;
    return const_cast<node *>( internal_find( n, strlen( n ) ) );
}

////////////////////////////////////////

const node *node::find( const char *n ) const
{
    if ( n == nullptr || n[0] == '\0' )
        return nullptr;
    return internal_find( n, strlen( n ) );
}

////////////////////////////////////////

const node *node::internal_find( const char *n, size_t nLeft ) const
{
    size_t l = _name.size();
    if ( nLeft >= l && 0 == strncmp( _name.c_str(), n, l ) )
    {
        if ( n[l] == '\0' )
            return this;

        if ( n[l] == '/' )
        {
            nLeft -= l + 1;
            n += l + 1;
            for ( auto i: _children )
            {
                const node *c = i->internal_find( n, nLeft );
                if ( c )
                    return c;
            }
        }
    }
    return nullptr;
}

////////////////////////////////////////

} // namespace scene
