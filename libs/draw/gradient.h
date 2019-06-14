// Copyright (c) 2016 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include "types.h"

#include <memory>
#include <vector>

namespace draw
{
////////////////////////////////////////

/// @brief Gradient.
class gradient
{
public:
    using value_type = float;
    using stop_type  = std::pair<value_type, color>;
    gradient( void ) {}

    gradient( const gradient &g ) : _stops( g._stops ) {}

    gradient( std::initializer_list<stop_type> l ) : _stops( l ) {}

    ~gradient( void ) {}

    void add_stop( value_type v, const color &c )
    {
        _stops.emplace_back( v, c );
    }

    void sort( void );

    color sample( value_type v ) const;

    const std::vector<stop_type> &stops( void ) const { return _stops; }

    gradient &operator=( const gradient &g )
    {
        _stops = g._stops;
        return *this;
    }

    void clear( void ) { _stops.clear(); }

private:
    gradient( gradient && ) = delete;

    std::vector<stop_type> _stops;
};

////////////////////////////////////////

} // namespace draw
