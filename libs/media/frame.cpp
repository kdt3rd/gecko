// Copyright (c) 2018 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "frame.h"

#include <base/contract.h>

////////////////////////////////////////

namespace media
{
////////////////////////////////////////

frame::~frame( void ) {}

////////////////////////////////////////

bool frame::layer_has_view( size_t layerIdx, size_t viewIdx ) const
{
    return ( layerIdx < _layers.size() ) ? _layers[layerIdx].has_view( viewIdx )
                                         : false;
}

////////////////////////////////////////

bool frame::layer_has_view( size_t layerIdx, base::cstring v ) const
{
    return ( layerIdx < _layers.size() ) ? _layers[layerIdx].has_view( v )
                                         : false;
}

////////////////////////////////////////

bool frame::layer_has_view( base::cstring layer, size_t viewIdx ) const
{
    for ( auto &l: _layers )
    {
        if ( l.name() == layer )
            return l.has_view( viewIdx );
    }
    return false;
}

////////////////////////////////////////

bool frame::layer_has_view( base::cstring layer, base::cstring v ) const
{
    for ( auto &l: _layers )
    {
        if ( l.name() == layer )
            return l.has_view( v );
    }
    return false;
}

////////////////////////////////////////

std::shared_ptr<image>
frame::find_image( size_t layerIdx, size_t viewIdx ) const
{
    if ( layerIdx < _layers.size() )
        return static_cast<std::shared_ptr<image>>(
            _layers[layerIdx][viewIdx] );
    return std::shared_ptr<image>();
}

////////////////////////////////////////

std::shared_ptr<image>
frame::find_image( size_t layerIdx, base::cstring v ) const
{
    if ( layerIdx < _layers.size() )
    {
        if ( v.empty() )
            return static_cast<std::shared_ptr<image>>(
                _layers[layerIdx].default_view() );

        return _layers[layerIdx].find_image( v );
    }
    return std::shared_ptr<image>();
}

////////////////////////////////////////

std::shared_ptr<image>
frame::find_image( base::cstring layer, size_t viewIdx ) const
{
    if ( layer.empty() )
        return find_image( 0, viewIdx );

    for ( auto &l: _layers )
    {
        if ( l.name() == layer )
            return static_cast<std::shared_ptr<image>>( l[viewIdx] );
    }
    return std::shared_ptr<image>();
}

////////////////////////////////////////

std::shared_ptr<image>
frame::find_image( base::cstring layer, base::cstring v ) const
{
    if ( layer.empty() )
        return find_image( 0, v );

    for ( auto &l: _layers )
    {
        if ( l.name() == layer )
            return l.find_image( v );
    }
    return std::shared_ptr<image>();
}

////////////////////////////////////////

std::shared_ptr<data> frame::find_data( size_t layerIdx, size_t viewIdx ) const
{
    if ( layerIdx < _layers.size() )
        return static_cast<std::shared_ptr<data>>( _layers[layerIdx][viewIdx] );
    return std::shared_ptr<data>();
}

////////////////////////////////////////

std::shared_ptr<data> frame::find_data( size_t layerIdx, base::cstring v ) const
{
    if ( layerIdx < _layers.size() )
    {
        if ( v.empty() )
            return static_cast<std::shared_ptr<data>>(
                _layers[layerIdx].default_view() );

        return _layers[layerIdx].find_data( v );
    }

    return std::shared_ptr<data>();
}

////////////////////////////////////////

std::shared_ptr<data>
frame::find_data( base::cstring layer, size_t viewIdx ) const
{
    if ( layer.empty() )
        return find_data( 0, viewIdx );

    for ( auto &l: _layers )
    {
        if ( l.name() == layer )
            return static_cast<std::shared_ptr<data>>( l[viewIdx] );
    }
    return std::shared_ptr<data>();
}

////////////////////////////////////////

std::shared_ptr<data>
frame::find_data( base::cstring layer, base::cstring v ) const
{
    if ( layer.empty() )
        return find_data( 0, v );

    for ( auto &l: _layers )
    {
        if ( l.name() == layer )
            return l.find_data( v );
    }
    return std::shared_ptr<data>();
}

////////////////////////////////////////

frame::named_image_list frame::images_for_view( base::cstring v ) const
{
    named_image_list ret;
    for ( auto &l: _layers )
    {
        auto i = l.find_image( v );
        if ( i )
            ret.emplace_back( std::make_pair( l.name(), i ) );
    }
    return ret;
}

////////////////////////////////////////

layer &frame::register_layer( base::cstring lname )
{
    for ( auto &l: _layers )
    {
        if ( l.name() == lname )
            throw_runtime( "Attempt to create layer with duplicate name" );
    }
    _layers.emplace_back( layer( lname ) );
    return _layers.back();
}

} // namespace media
