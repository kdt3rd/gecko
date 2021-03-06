// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "registry.h"

#include "float_ops.h"

#include <base/contract.h>
#include <limits>

////////////////////////////////////////

namespace engine
{
////////////////////////////////////////

registry::registry( void )
{
    register_constant<bool>();

    register_constant<float>();
    register_constant<double>();

    register_constant<uint8_t>();
    register_constant<uint16_t>();
    register_constant<uint32_t>();
    register_constant<uint64_t>();

    register_constant<int8_t>();
    register_constant<int16_t>();
    register_constant<int32_t>();
    register_constant<int64_t>();

    register_constant<std::string>();
    register_constant<std::vector<float>>();
    register_constant<std::vector<double>>();

    register_float_ops( *this );
}

////////////////////////////////////////

registry::~registry( void ) {}

////////////////////////////////////////

op_id registry::find( const base::cstring &n ) const
{
    auto i = _name_to_op.find( n );
    if ( i == _name_to_op.end() )
        throw_logic( "No operation by name of {0} found", n );

    return i->second;
}

////////////////////////////////////////

op_id registry::find_constant( const std::type_info &ti ) const
{
    // name is not guaranteed to be unique, but neither is hash_code,
    // but should be good enough for the provided types
    return find( ti.name() );
}

////////////////////////////////////////

op_id registry::add( const op &o )
{
    precondition(
        ( _ops.size() + 1 ) <
            static_cast<size_t>( std::numeric_limits<op_id>::max() ),
        "Too many operations registered for size of op_id" );

    op_id r = static_cast<op_id>( _ops.size() );

    if ( _name_to_op.find( o.name() ) != _name_to_op.end() )
        throw_logic(
            "Operation by the name of {0} already registered", o.name() );

    _name_to_op[o.name()] = r;
    _ops.push_back( o );
    return r;
}

////////////////////////////////////////

op_id registry::add( op &&o )
{
    precondition(
        ( _ops.size() + 1 ) <
            static_cast<size_t>( std::numeric_limits<op_id>::max() ),
        "Too many operations registered for size of op_id" );

    op_id r = static_cast<op_id>( _ops.size() );
    if ( _name_to_op.find( o.name() ) != _name_to_op.end() )
        throw_logic(
            "Operation by the name of {0} already registered", o.name() );

    _name_to_op[o.name()] = r;
    _ops.emplace_back( std::move( o ) );
    return r;
}

////////////////////////////////////////

registry &registry::get( void )
{
    static registry base;
    return base;
}

////////////////////////////////////////

} // namespace engine
