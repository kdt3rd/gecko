// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "box.h"

#include <base/contract.h>

namespace layout
{
////////////////////////////////////////

box::box( alignment direction ) : _align( direction )
{
    precondition(
        direction != alignment::CENTER, "invalid direction for box layout" );
}

////////////////////////////////////////

void box::compute_bounds( void )
{
    // Clean up areas that have been deleted.
    _areas.remove_if(
        []( const std::weak_ptr<area> &a ) { return a.expired(); } );

    coord minw = min_coord();
    coord minh = min_coord();
    coord maxw = min_coord();
    coord maxh = min_coord();
    switch ( _align )
    {
        case alignment::LEFT:
        case alignment::RIGHT:
            for ( auto &wa: _areas )
            {
                auto a = wa.lock();
                if ( a )
                {
                    a->compute_bounds();
                    minw += a->minimum_width();
                    minh = std::max( minh, a->minimum_height() );
                    maxw += a->maximum_width();
                    maxh = std::max( maxh, a->maximum_height() );
                }
            }
            if ( !_areas.empty() )
            {
                minw += _spacing[0] * ( _areas.size() - 1 );
                maxw += _spacing[0] * ( _areas.size() - 1 );
            }
            break;

        case alignment::TOP:
        case alignment::BOTTOM:
            for ( auto &wa: _areas )
            {
                auto a = wa.lock();
                if ( a )
                {
                    a->compute_bounds();
                    minw = std::max( minw, a->minimum_width() );
                    minh += a->minimum_height();
                    maxw = std::max( maxw, a->maximum_width() );
                    maxh += a->maximum_height();
                }
            }
            if ( !_areas.empty() )
            {
                minh += _spacing[1] * ( _areas.size() - 1 );
                maxh += _spacing[1] * ( _areas.size() - 1 );
            }
            break;

        case alignment::TOP_LEFT:
        case alignment::TOP_RIGHT:
        case alignment::BOTTOM_LEFT:
        case alignment::BOTTOM_RIGHT:
            for ( auto &wa: _areas )
            {
                auto a = wa.lock();
                if ( a )
                {
                    a->compute_bounds();
                    minw += a->minimum_width();
                    minh += a->minimum_height();
                    maxw += a->maximum_width();
                    maxh += a->maximum_height();
                }
            }
            if ( !_areas.empty() )
            {
                minw += _spacing[0] * ( _areas.size() - 1 );
                minh += _spacing[1] * ( _areas.size() - 1 );
                maxw += _spacing[0] * ( _areas.size() - 1 );
                maxh += _spacing[1] * ( _areas.size() - 1 );
            }
            break;

        case alignment::CENTER:
            throw_runtime( "invalid direction for box layout" );
    }
    minw += _pad[0] + _pad[1];
    minh += _pad[2] + _pad[3];
    maxw += _pad[0] + _pad[1];
    maxh += _pad[2] + _pad[3];
    if ( _areas.empty() )
    {
        maxw = max_coord();
        maxh = max_coord();
    }

    set_minimum( minw, minh );
    set_maximum( maxw, maxh );
}

////////////////////////////////////////

void box::compute_layout( void )
{
    coord x = min_coord(), y = min_coord();
    coord dx = min_coord(), dy = min_coord();

    switch ( _align )
    {
        case alignment::LEFT:
            x  = _pad[0];
            y  = _pad[2];
            dx = coord( 1 );
            dy = min_coord();
            break;

        case alignment::RIGHT:
            x  = width() - _pad[1];
            y  = _pad[2];
            dx = coord( -1 );
            dy = min_coord();
            break;

        case alignment::TOP:
            x  = _pad[0];
            y  = height() - _pad[3];
            dx = min_coord();
            dy = coord( -1 );
            break;

        case alignment::BOTTOM:
            x  = _pad[0];
            y  = _pad[2];
            dx = min_coord();
            dy = coord( 1 );
            break;

        case alignment::TOP_LEFT:
            x  = width() - _pad[1];
            y  = height() - _pad[3];
            dx = coord( -1 );
            dy = coord( -1 );
            break;

        case alignment::TOP_RIGHT:
            x  = _pad[0];
            y  = height() - _pad[3];
            dx = coord( 1 );
            dy = coord( -1 );
            break;

        case alignment::BOTTOM_LEFT:
            x  = width() - _pad[1];
            y  = _pad[2];
            dx = coord( -1 );
            dy = coord( 1 );
            break;

        case alignment::BOTTOM_RIGHT:
            x  = _pad[0];
            y  = _pad[2];
            dx = coord( 1 );
            dy = coord( 1 );
            break;

        case alignment::CENTER:
            throw_runtime( "invalid direction for box layout" );
    }

    for ( auto &wa: _areas )
    {
        auto a = wa.lock();
        if ( a )
        {
            a->set_size( a->minimum_size() );
            a->compute_layout();
        }
    }

    coord w = std::max( min_coord(), width() - _pad[0] - _pad[1] );
    coord h = std::max( min_coord(), height() - _pad[2] - _pad[3] );

    if ( dx != min_coord() )
    {
        expand_width( _areas, width() - minimum_width() );
        w = min_coord();
    }
    if ( dy != min_coord() )
    {
        expand_height( _areas, height() - minimum_height() );
        h = min_coord();
    }

    for ( auto &wa: _areas )
    {
        auto a = wa.lock();
        if ( a )
        {
            coord aw = a->width();
            coord ah = a->height();
            a->set( { x1() + x, y1() + y }, { aw * dx + w, ah * dy + h } );
            x += ( aw + _spacing[0] ) * dx;
            y += ( ah + _spacing[1] ) * dy;
            a->compute_layout();
        }
    }
}

////////////////////////////////////////

} // namespace layout
