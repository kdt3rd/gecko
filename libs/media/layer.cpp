// Copyright (c) 2018 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "layer.h"

#include <base/contract.h>

////////////////////////////////////////

namespace media
{
////////////////////////////////////////

const view &layer::operator[]( base::cstring v ) const
{
    for ( auto &vs: _views )
    {
        if ( vs.name() == v )
            return vs;
    }
    throw_logic( "View name {} not in layer", v );
}

////////////////////////////////////////

const std::string &layer::default_view_name( void ) const
{
    static std::string empty_view;
    if ( _views.empty() )
        return empty_view;
    return _views[0].name();
}

////////////////////////////////////////

const view &layer::default_view( void ) const
{
    precondition( !_views.empty(), "no views defined for layer" );
    return _views[0];
}

////////////////////////////////////////

view &layer::add_view( base::cstring nm )
{
    for ( auto &vs: _views )
    {
        if ( vs.name() == nm )
            throw_logic( "View name {} already in layer", nm );
    }
    _views.emplace_back( view( nm ) );
    return _views.back();
}

////////////////////////////////////////

bool layer::has_view( size_t viewIdx ) const { return viewIdx < _views.size(); }

////////////////////////////////////////

bool layer::has_view( base::cstring v ) const
{
    for ( auto &vs: _views )
    {
        if ( vs.name() == v )
            return true;
    }
    return false;
}

////////////////////////////////////////

std::shared_ptr<image> layer::find_image( base::cstring v ) const
{
    for ( auto &vs: _views )
    {
        if ( vs.name() == v )
            return static_cast<std::shared_ptr<image>>( vs );
    }
    return std::shared_ptr<image>();
}

////////////////////////////////////////

std::shared_ptr<data> layer::find_data( base::cstring v ) const
{
    for ( auto &vs: _views )
    {
        if ( vs.name() == v )
            return static_cast<std::shared_ptr<data>>( vs );
    }
    return std::shared_ptr<data>();
}

////////////////////////////////////////

} // namespace media
