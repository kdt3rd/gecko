// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include "op.h"
#include "types.h"

#include <base/const_string.h>
#include <vector>

////////////////////////////////////////

namespace engine
{
///
/// @brief Class registry provides...
///
class registry
{
public:
    registry( void );
    ~registry( void );

    op_id find( const base::cstring &n ) const;
    op_id find_constant( const std::type_info &ti ) const;

    op_id                      add( const op &o );
    op_id                      add( op &&o );
    template <typename T> void register_constant( void );

    inline const op &get( op_id i ) const;
    inline const op &operator[]( op_id i ) const;

    /// global registry that is used by default for operators and in
    /// scenarios where it is inconvenient to pass a registry around
    /// for use with computed_value
    ///
    /// this singleton does not have to be used, graph and others can
    /// take a custom registry
    static registry &get( void );

private:
    std::vector<op>              _ops;
    std::map<std::string, op_id> _name_to_op;
};

////////////////////////////////////////

template <typename T> void registry::register_constant( void )
{
    // name is not guaranteed to be unique, but neither is hash_code,
    // but should be good enough for the expected types
    const std::type_info &ti = typeid( T );
    add( op( ti.name(), ti, op::value ) );
}

////////////////////////////////////////

inline const op &registry::get( op_id i ) const { return _ops[i]; }

inline const op &registry::operator[]( op_id i ) const { return get( i ); }

} // namespace engine
