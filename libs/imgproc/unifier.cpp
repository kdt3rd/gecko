// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "unifier.h"

#include <algorithm>

namespace imgproc
{
////////////////////////////////////////

void unifier::operator()( const type_variable &x, const type_variable &y )
{
    if ( x != y )
        eliminate( x, y );
}

////////////////////////////////////////

void unifier::operator()( const type_variable &x, const type_primary &y )
{
    if ( occurs( y, x ) )
        throw_runtime( "..." );
    eliminate( x, y );
}

////////////////////////////////////////

void unifier::operator()( const type_variable &x, const type_callable &y )
{
    if ( occurs( y, x ) )
        throw_runtime( "..." );
    eliminate( x, y );
}

////////////////////////////////////////

void unifier::operator()( const type_primary &x, const type_variable &y )
{
    if ( occurs( x, y ) )
        throw_runtime( "..." );
    eliminate( y, x );
}

////////////////////////////////////////

void unifier::operator()( const type_primary &x, const type_primary &y )
{
    if ( !x.is_compatible( y ) )
        throw_runtime( "type mismatch: {0} and {1}", x, y );
}

////////////////////////////////////////

[[noreturn]] void unifier::
                  operator()( const type_primary &x, const type_callable &y )
{
    throw_runtime( "assigning function to variable: {0} and {1}", x, y );
}

////////////////////////////////////////

void unifier::operator()( const type_callable &x, const type_variable &y )
{
    if ( occurs( x, y ) )
        throw_runtime( "..." );
    eliminate( y, x );
}

////////////////////////////////////////

[[noreturn]] void unifier::
                  operator()( const type_callable &x, const type_primary &y )
{
    throw_runtime( "assigning variable to function: {0} and {1}", x, y );
}

////////////////////////////////////////

void unifier::operator()( const type_callable &x, const type_callable &y )
{
    precondition(
        x.size() == y.size(),
        "function mismatch with number of arguments: {0} and {1}",
        x,
        y );
    for ( size_t i = 0; i < x.size(); ++i )
        visit( *this, x.at( i ), y.at( i ) );
    visit( *this, y.get_result(), x.get_result() );
}

////////////////////////////////////////

void unifier::add_constraint( const type &t1, const type &t2 )
{
    _stack.emplace_back( t1, t2 );
    if ( t1.is<type_callable>() && t2.is<type_callable>() )
    {
        auto &tmp1 = t1.get<type_callable>();
        auto &tmp2 = t2.get<type_callable>();
        for ( size_t i = 0; i < tmp1.size() && i < tmp2.size(); ++i )
            add_constraint( tmp1.at( i ), tmp2.at( i ) );
        add_constraint( tmp1.get_result(), tmp2.get_result() );
    }
}

////////////////////////////////////////

type unifier::get( const type &tv )
{
    type next( tv );
    while ( next.is<type_variable>() )
    {
        auto tmp = next.get<type_variable>();
        auto i   = _substitution.find( tmp );
        if ( i == _substitution.end() )
            next.clear();
        else
            next = i->second;
    }

    return next;
}

////////////////////////////////////////

void unifier::unify( void )
{
    while ( !_stack.empty() )
    {
        type x = std::move( _stack.back().first );
        type y = std::move( _stack.back().second );
        _stack.pop_back();
        visit( *this, x, y );
    }
}

////////////////////////////////////////

void unifier::eliminate( const type_variable &x, const type &y )
{
    for ( auto &i: _stack )
    {
        replace( i.first, x, y );
        replace( i.second, x, y );
    }

    for ( auto &i: _substitution )
        replace( i.second, x, y );

    _substitution[x] = y;
}

////////////////////////////////////////

void unifier::replace(
    type &x, const type_variable &target, const type &replacement )
{
    if ( x.is<type_callable>() )
    {
        // recursively replace arguments and result types
        auto &op = x.get<type_callable>();
        for ( auto &o: op )
            replace( o, target, replacement );
        replace( op.get_result(), target, replacement );
    }
    else if ( x.is<type_primary>() )
    {
        // no replacement for primary type.
    }
    else
    {
        // replace the variable if it is the target.
        auto &var = x.get<type_variable>();
        if ( var == target )
            x = replacement;
    }
}

////////////////////////////////////////

bool unifier::occurs( const type &haystack, const type_variable &needle )
{
    bool result = false;

    if ( haystack.is<type_callable>() )
    {
        // recursively check args and result types.
        auto &op = haystack.get<type_callable>();
        result   = std::any_of( op.begin(), op.end(), [&]( const type &x ) {
            return occurs( x, needle );
        } );

        if ( !result )
            return occurs( op.get_result(), needle );
    }
    else if ( haystack.is<type_variable>() )
    {
        auto &var = haystack.get<type_variable>();
        result    = ( var == needle );
    }

    return result;
}

////////////////////////////////////////

} // namespace imgproc
