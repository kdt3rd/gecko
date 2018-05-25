//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "packing.h"
#include <base/contract.h>
#include <base/reverse.h>

namespace layout
{

////////////////////////////////////////

packing::packing( void )
{
}

////////////////////////////////////////

void packing::add( const std::shared_ptr<area> &a, alignment where )
{
	precondition( _areas.empty() || _areas.back()._align != alignment::CENTER, "center packing must be used last" );
	_areas.emplace_back( a, where );
}

////////////////////////////////////////

void packing::compute_bounds( void )
{
	rect bmin, bmax;

	// To compute the minimum size, we build backwards from the inside out.
	for ( auto &s: base::reverse( _areas ) )
	{
		auto a = s._area.lock();
		if ( a )
		{
			a->compute_bounds();
			switch ( s._align )
			{
				case alignment::CENTER:
					bmin.set_size( a->minimum_size() );
					bmax.set_size( a->maximum_size() );
					break;

				case alignment::TOP:
					if ( !bmin.empty() )
						bmin.grow( 0, 0, _spacing[1], 0 );
					if ( !bmax.empty() )
						bmax.grow( 0, 0, _spacing[1], 0 );
					bmin.grow( 0, 0, a->minimum_height(), 0 );
					bmax.grow( 0, 0, a->maximum_height(), 0 );
					if ( bmin.width() < a->minimum_width() )
						bmin.set_width( a->minimum_width() );
					if ( bmax.width() < a->maximum_width() )
						bmax.set_width( a->maximum_width() );
					break;

				case alignment::TOP_LEFT:
					if ( !bmin.empty() )
						bmin.grow( _spacing[0], 0, _spacing[1], 0 );
					if ( !bmax.empty() )
						bmax.grow( _spacing[0], 0, _spacing[1], 0 );
					bmin.grow( a->minimum_width(), 0, a->minimum_height(), 0 );
					bmax.grow( a->maximum_width(), 0, a->maximum_height(), 0 );
					break;

				case alignment::TOP_RIGHT:
					if ( !bmin.empty() )
						bmin.grow( 0, _spacing[0], _spacing[1], 0 );
					if ( !bmax.empty() )
						bmax.grow( 0, _spacing[0], _spacing[1], 0 );
					bmin.grow( 0, a->minimum_width(), a->minimum_height(), 0 );
					bmax.grow( 0, a->maximum_width(), a->maximum_height(), 0 );
					break;

				case alignment::BOTTOM:
					if ( !bmin.empty() )
						bmin.grow( 0, 0, 0, _spacing[1] );
					if ( !bmax.empty() )
						bmax.grow( 0, 0, 0, _spacing[1] );
					bmin.grow( 0, 0, 0, a->minimum_height() );
					bmax.grow( 0, 0, 0, a->maximum_height() );
					if ( bmin.width() < a->minimum_width() )
						bmin.set_width( a->minimum_width() );
					if ( bmax.width() < a->maximum_width() )
						bmax.set_width( a->maximum_width() );
					break;

				case alignment::BOTTOM_RIGHT:
					if ( !bmin.empty() )
						bmin.grow( 0, _spacing[0], 0, _spacing[1] );
					if ( !bmax.empty() )
						bmax.grow( 0, _spacing[0], 0, _spacing[1] );
					bmin.grow( 0, a->minimum_width(), 0, a->minimum_height() );
					bmax.grow( 0, a->maximum_width(), 0, a->maximum_height() );
					break;

				case alignment::BOTTOM_LEFT:
					if ( !bmin.empty() )
						bmin.grow( _spacing[0], 0, 0, _spacing[1] );
					if ( !bmax.empty() )
						bmax.grow( _spacing[0], 0, 0, _spacing[1] );
					bmin.grow( a->minimum_width(), 0, 0, a->minimum_height() );
					bmax.grow( a->maximum_width(), 0, 0, a->maximum_height() );
					break;

				case alignment::RIGHT:
					if ( !bmin.empty() )
						bmin.grow( 0, _spacing[0], 0, 0 );
					if ( !bmax.empty() )
						bmax.grow( 0, _spacing[0], 0, 0 );
					bmin.grow( 0, a->minimum_width(), 0, 0 );
					bmax.grow( 0, a->maximum_width(), 0, 0 );
					if ( bmin.height() < a->minimum_height() )
						bmin.set_height( a->minimum_height() );
					if ( bmax.height() < a->maximum_height() )
						bmax.set_height( a->maximum_height() );
					break;

				case alignment::LEFT:
					if ( !bmin.empty() )
						bmin.grow( _spacing[0], 0, 0, 0 );
					if ( !bmax.empty() )
						bmax.grow( _spacing[0], 0, 0, 0 );
					bmin.grow( a->minimum_width(), 0, 0, 0 );
					bmax.grow( a->maximum_width(), 0, 0, 0 );
					if ( bmin.height() < a->minimum_height() )
						bmin.set_height( a->minimum_height() );
					if ( bmax.height() < a->maximum_height() )
						bmax.set_height( a->maximum_height() );
					break;
			}
		}
	}

	bmin.grow( _pad[0], _pad[1], _pad[2], _pad[3] );
	bmax.grow( _pad[0], _pad[1], _pad[2], _pad[3] );

	set_minimum( bmin.width(), bmin.height() );
	set_maximum( bmax.width(), bmax.height() );
}

////////////////////////////////////////

void packing::compute_layout( void )
{
	// Clean up areas that have been deleted.
	_areas.remove_if( []( const section &s ) { return s._area.expired(); } );

	coord x = _pad[0];
	coord y = _pad[2];
	coord w = std::max( width(), minimum_width() ) - _pad[0] - _pad[1];
	coord h = std::max( height(), minimum_height() ) - _pad[2] - _pad[3];

	for ( auto &s: _areas )
	{
		auto a = s._area.lock();
		if ( a )
		{
			if ( w <= min_coord() || h <= min_coord() )
			{
				a->set( { x1() + x, y1() + y }, { w, h } );
				a->compute_layout();
				continue;
			}

			coord aw = a->minimum_width();
			coord ah = a->minimum_height();
			bool top = false;
			bool bottom = false;
			bool left = false;
			bool right = false;

			switch ( s._align )
			{
				// Fill the last remaining space
				case alignment::CENTER:
					a->set( { x1() + x, y1() + y }, { w, h } );
					x += divide( w, coord(2) );
					y += divide( h, coord(2) );
					w = min_coord();
					h = min_coord();
					top = true;
					bottom = true;
					left = true;
					right = true;
					break;

				// Fill the space at the top
				case alignment::TOP:
					a->set( { x1() + x, y1() + y }, { w, ah } );
					ah += ( top ? _spacing[1] : min_coord() );
					y += ah;
					h -= ah;
					top = true;
					break;

				// Fill the space at the top left
				case alignment::TOP_LEFT:
					a->set( { x1() + x, y1() + y }, { aw, ah } );
					aw += ( left ? _spacing[0] : min_coord() );
					ah += ( top ? _spacing[1] : min_coord() );
					x += aw;
					y += ah;
					w -= aw;
					h -= ah;
					top = true;
					left = true;
					break;

				case alignment::TOP_RIGHT:
					a->set( { x1() + x, y1() + y + h - ah }, { aw, ah } );
					aw += ( right ? _spacing[0] : min_coord() );
					ah += ( top ? _spacing[1] : min_coord() );
					y += ah;
					w -= aw;
					h -= ah;
					top = true;
					right = true;
					break;

				case alignment::BOTTOM:
					a->set( { x1() + x, y1() + y + h - ah }, { w, ah } );
					ah += ( bottom ? _spacing[1] : min_coord() );
					h -= ah;
					bottom = true;
					break;

				case alignment::BOTTOM_RIGHT:
					a->set( { x1() + x + w - aw, y1() + y + h - ah }, { aw, ah } );
					aw += ( right ? _spacing[0] : min_coord() );
					ah += ( bottom ? _spacing[1] : min_coord() );
					w -= aw;
					h -= ah;
					bottom = true;
					right = true;
					break;

				case alignment::BOTTOM_LEFT:
					a->set( { x1() + x, y1() + y + h - ah }, { aw, ah } );
					aw += ( left ? _spacing[0] : min_coord() );
					ah += ( bottom ? _spacing[1] : min_coord() );
					x += aw;
					w -= aw;
					h -= ah;
					bottom = true;
					left = true;
					break;

				case alignment::RIGHT:
					a->set( { x1() + x + w - aw, y1() + y }, { aw, h } );
					aw += ( right ? _spacing[0] : min_coord() );
					w -= aw;
					right = true;
					break;

				case alignment::LEFT:
					a->set( { x1() + x, y1() + y }, { aw, h } );
					aw += ( right ? _spacing[0] : min_coord() );
					x += aw;
					w -= aw;
					left = true;
					break;
			}
			if ( top )
			{
				y += _spacing[1];
				h -= _spacing[1];
			}
			if ( left )
			{
				x += _spacing[0];
				w -= _spacing[0];
			}
			if ( bottom )
				h -= _spacing[1];
			if ( right )
				w -= _spacing[0];
			a->compute_layout();
		}
	}
}

////////////////////////////////////////

}

