// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <iterator>

////////////////////////////////////////

namespace scene
{
class node;

/// This provides a consistent view of a node during the iterator lifetime
/// NB: if you implement a recursive algorithm to traverse the tree, you
/// may end up locking out any other threads from manipulating the
class const_node_iterator
    : public std::iterator<std::random_access_iterator_tag, size_t>
{
    typedef std::iterator_traits<I> _traits;

public:
    using difference_type = typename _traits::difference_type;
    /// unary operators

    node_reference  operator*(void)const noexcept;
    node_reference *operator->(void)const noexcept;
    node_iterator & operator++( void );
    node_iterator   operator++( int );
    node_iterator & operator--( void );
    node_iterator   operator--( int );

    column_iterator &operator+=( difference_type i );
    column_iterator &operator-=( difference_type i );
    column_iterator  operator+( difference_type i ) const;
    column_iterator  operator-( difference_type i ) const;

    difference_type operator-( const const_node_iterator &i ) const;

    node_reference operator[]( difference_type i ) const;
};

class node_reference;

/// @brief accessor that provides non-const access to a scene graph node
///
/// These objects are expected to be short-lived, surviving only long
/// enough to trigger some small change that is acted upon by a
/// different actor.
///
/// The lifetime of this object must be strictly less than the
/// lifetime of the node_reference it is created from. Anything else
/// results in undefined behavior.
///
/// As such, the functionality provided by this is minimal, only
/// enough to track the write lock on the node and give a non-const
/// function call onto the node.
///
class writable_node_ref
{
public:
    writable_node_ref( void ) = default;
    inline writable_node_ref( node_reference &nr );
    inline ~writable_node_ref( void );
    inline writable_node_ref( writable_node_ref &&r ) noexcept;
    inline writable_node_ref &operator=( writable_node_ref &&r ) noexcept;
    // move-only on a writable reference
    writable_node_ref( const writable_node_ref &r ) = delete;
    writable_node_ref &operator=( const writable_node_ref &r ) = delete;

    inline node &operator*(void)const noexcept;
    inline node *operator->(void)const noexcept;

    inline explicit operator bool( void ) const;

    template <typename T> inline T *as( void ) const;

private:
    node_reference *_node_ref = nullptr;
};

/// @brief read-only node reference.
///
/// This provides thread-safe access to the scene tree, where only
/// these and @sa writable_node_ref as a subset of objects are
/// supposed to have access to the scene graph as a public API. Of
/// course, one can be tricky and stash off the pointer from a get()
/// call here, but that voids any claim of thread safety, so be
/// careful where that is done, if at all.
///
/// TBD: allow access to this object while we have a writable_node_ref created? no
/// problem with it, but is it confusing?
class node_reference
{
public:
    using const_iterator = const_node_iterator;

    constexpr node_reference( nullptr_t ) noexcept = default;
    constexpr node_reference( void ) noexcept      = default;
    ~node_reference( void )
    {
        if ( _node )
            _node->read_unlock();
    }

    node_reference( const node_reference &n )
        : _scene( n._scene ), _node( nullptr )
    {
        // don't assign the node until we safely have the read lock
        if ( n._node )
        {
            n._node->read_lock();
            _node = n._node;
        }
    }

    node_reference &operator=( const node_reference &n ) noexcept;
    node_reference( node_reference &&n ) noexcept;
    node_reference &operator=( node_reference &&n ) noexcept;

    node_reference parent( void )
    {
        if ( _node )
            return node_reference( _scene, _node->parent() );
        return node_reference();
    }

    size_t size( void ) const { return _node ? _node->children().size() : 0; }
    node_reference operator[]( size_t i ) const
    {
        return node_reference( _scene, _node ? _node->children()[i] : nullptr );
    }
    node_reference operator[]( const char *n ) const
    {
        return node_reference( _scene, _node ? _node->find( n ) : nullptr );
    }

    const_iterator begin( void ) const;
    const_iterator end( void ) const;

    const node *get( void ) const noexcept { return _node; }
    const node &operator*(void)const noexcept { return *get(); }
    const node *operator->(void)const noexcept { return get(); }

    explicit operator bool( void ) const { return _node != nullptr; }

    template <typename T> const T *as( void ) const
    {
        return dynamic_cast<const T *>( _node->implementation() );
    }

    inline writable_node_ref make_writable( void )
    {
        return writable_node_ref( *this );
    }

protected:
    explicit node_reference(
        scene &s, node_reference &&parent, node *n ) noexcept
        : _scene( s ), _node( nullptr )
    {
        if ( n )
        {
            n->read_lock();
            _node = n;
        }
    }

    node *get_raw( void ) const { return _node; }
    void  checkout_writable( void );
    void  checkin_writable( void );

    friend class scene;
    friend class writable_node_ref;

    scene *_scene = nullptr;
    node * _node  = nullptr;
};

////////////////////////////////////////

void swap( node_reference &a, node_reference &b );

////////////////////////////////////////////////////////////////////////////////
// implementation after class definition to avoid logical loop in definition order

inline writable_node_ref::writable_node_ref( node_reference &r )
{
    r.checkout_writable();
    _node_ref = &r;
}
inline writable_node_ref::writable_node_ref( writable_node_ref &&r )
    : _node_ref( base::exchange( _node_ref, nullptr ) )
{}

inline writable_node_ref &writable_node_ref::operator=( writable_node_ref &&r )
{
    _node_ref = base::exchange( r._node_ref, _node_ref );
    return *this;
}
inline writable_node_ref::~writable_node_ref( void )
{
    if ( _node_ref )
        _node_ref->checkin_writable();
}

inline node &writable_node_ref::operator*(void)const noexcept
{
    return *( _node_ref->get_raw() );
}

inline node *writable_node_ref::operator->(void)const noexcept
{
    return _node_ref->get_raw();
}

inline writable_node_ref::operator bool( void ) const
{
    return _node_ref && _node_ref->get_raw() != nullptr;
}

template <typename T> inline T *writable_node_ref::as( void ) const
{
    return dynamic_cast<T *>( _node_ref.get_raw()->implementation() );
}

} // namespace scene
