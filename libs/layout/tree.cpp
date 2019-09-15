// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "tree.h"

#include <base/format.h>

namespace layout
{
////////////////////////////////////////

void tree::compute_bounds( void )
{
    auto g = _groove.lock();
    auto t = _title.lock();

    coord minw = min_coord();
    coord minh = min_coord();
    coord maxw = min_coord();
    coord maxh = min_coord();

    if ( t )
    {
        t->compute_bounds();
        minw = std::max( minw, t->minimum_width() );
        minh += t->minimum_height();
        maxw = std::max( maxw, t->maximum_width() );
        maxh += t->maximum_height();
    }
    for ( auto &w: _areas )
    {
        auto a = w.lock();
        if ( a )
        {
            a->compute_bounds();
            if ( std::dynamic_pointer_cast<tree>( a ) )
            {
                minw = std::max( minw, a->minimum_width() );
                maxw = std::max( maxw, a->maximum_width() );
            }
            else
            {
                minw = std::max(
                    minw, a->minimum_width() + _indent + _spacing[0] );
                maxw = std::max(
                    maxw, a->maximum_width() + _indent + _spacing[0] );
            }
            minh += a->minimum_height();
            maxh += a->maximum_height();
        }
    }
    if ( !_areas.empty() )
    {
        minh += ( _areas.size() - 1 ) * _spacing[1];
        maxh += ( _areas.size() - 1 ) * _spacing[1];
    }
    if ( g )
    {
        g->compute_bounds();
        minw += std::max( _indent, g->minimum_width() + _spacing[0] );
        minh = std::max( minh, g->minimum_height() );
        maxw += std::max( _indent, g->maximum_width() + _spacing[0] );
        maxh = std::max( maxh, g->maximum_height() );
    }

    minw += _pad[0] + _pad[1];
    minh += _pad[2] + _pad[3] + _spacing[1];
    maxw += _pad[0] + _pad[1];
    maxh += _pad[2] + _pad[3] + _spacing[1];

    set_minimum( minw, minh );
    set_maximum( maxw, maxh );
}

////////////////////////////////////////

void tree::compute_layout( void )
{
    auto g = _groove.lock();
    auto t = _title.lock();

    coord iw = _indent;
    if ( g )
        iw =
            std::max( _indent + _spacing[0], g->minimum_width() + _spacing[0] );

    coord cw = std::max( min_coord(), width() - _pad[0] - _pad[1] - iw );
    coord th = min_coord();
    if ( t )
        th = t->minimum_height();

    coord ch = height() - _pad[2] - _pad[3] - _spacing[1] - th;

    coord x = _pad[0];
    coord y = _pad[2];
    if ( g )
    {
        g->set(
            { x1() + x, y1() + y },
            { iw - _spacing[0], th + _spacing[1] + ch } );
        g->compute_layout();
    }
    x += iw;
    if ( t )
    {
        t->set( { x1() + x, y1() + y }, { cw, th } );
        t->compute_layout();
    }
    y += th + _spacing[1];
    for ( auto &w: _areas )
    {
        auto a = w.lock();
        if ( a )
        {
            if ( std::dynamic_pointer_cast<tree>( a ) )
            {
                a->set( { x1() + x, y1() + y }, { cw, a->minimum_height() } );
                a->compute_layout();
            }
            else
            {
                a->set(
                    { x1() + x + _indent + _spacing[0], y1() + y },
                    { cw - _indent - _spacing[0], a->minimum_height() } );
                a->compute_layout();
            }
            y += a->minimum_height() + _spacing[1];
        }
    }
}

////////////////////////////////////////

} // namespace layout
