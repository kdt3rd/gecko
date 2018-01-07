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

void packing::add( const std::shared_ptr<area> &a, base::alignment where )
{
	precondition( _areas.empty() || _areas.back()._align != base::alignment::CENTER, "center packing must be used last" );
	_areas.emplace_back( a, where );
}

////////////////////////////////////////

void packing::compute_bounds( void )
{
	base::drect bmin, bmax;

	// To compute the minimum size, we build backwards from the inside out.
	for ( auto &s: base::reverse( _areas ) )
	{
		auto a = s._area.lock();
		if ( a )
		{
			a->compute_bounds();
			switch ( s._align )
			{
				case base::alignment::CENTER:
					bmin.set_size( a->minimum_size() );
					bmax.set_size( a->maximum_size() );
					break;

				case base::alignment::TOP:
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

				case base::alignment::TOP_LEFT:
					if ( !bmin.empty() )
						bmin.grow( _spacing[0], 0, _spacing[1], 0 );
					if ( !bmax.empty() )
						bmax.grow( _spacing[0], 0, _spacing[1], 0 );
					bmin.grow( a->minimum_width(), 0, a->minimum_height(), 0 );
					bmax.grow( a->maximum_width(), 0, a->maximum_height(), 0 );
					break;

				case base::alignment::TOP_RIGHT:
					if ( !bmin.empty() )
						bmin.grow( 0, _spacing[0], _spacing[1], 0 );
					if ( !bmax.empty() )
						bmax.grow( 0, _spacing[0], _spacing[1], 0 );
					bmin.grow( 0, a->minimum_width(), a->minimum_height(), 0 );
					bmax.grow( 0, a->maximum_width(), a->maximum_height(), 0 );
					break;

				case base::alignment::BOTTOM:
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

				case base::alignment::BOTTOM_RIGHT:
					if ( !bmin.empty() )
						bmin.grow( 0, _spacing[0], 0, _spacing[1] );
					if ( !bmax.empty() )
						bmax.grow( 0, _spacing[0], 0, _spacing[1] );
					bmin.grow( 0, a->minimum_width(), 0, a->minimum_height() );
					bmax.grow( 0, a->maximum_width(), 0, a->maximum_height() );
					break;

				case base::alignment::BOTTOM_LEFT:
					if ( !bmin.empty() )
						bmin.grow( _spacing[0], 0, 0, _spacing[1] );
					if ( !bmax.empty() )
						bmax.grow( _spacing[0], 0, 0, _spacing[1] );
					bmin.grow( a->minimum_width(), 0, 0, a->minimum_height() );
					bmax.grow( a->maximum_width(), 0, 0, a->maximum_height() );
					break;

				case base::alignment::RIGHT:
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

				case base::alignment::LEFT:
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

	double x = _pad[0];
	double y = _pad[2];
	double w = std::max( width(), minimum_width() ) - _pad[0] - _pad[1];
	double h = std::max( height(), minimum_height() ) - _pad[2] - _pad[3];

	for ( auto &s: _areas )
	{
		auto a = s._area.lock();
		if ( a )
		{
			if ( w <= 0.0 || h <= 0.0 )
			{
				a->set( { x1() + x, y1() + y }, { w, h } );
				a->compute_layout();
				continue;
			}

			double aw = a->minimum_width();
			double ah = a->minimum_height();
			bool top = false;
			bool bottom = false;
			bool left = false;
			bool right = false;

			switch ( s._align )
			{
				// Fill the last remaining space
				case base::alignment::CENTER:
					a->set( { x1() + x, y1() + y }, { w, h } );
					x += w / 2.0;
					y += h / 2.0;
					w = 0.0;
					h = 0.0;
					top = true;
					bottom = true;
					left = true;
					right = true;
					break;

				// Fill the space at the top
				case base::alignment::TOP:
					a->set( { x1() + x, y1() + y }, { w, ah } );
					ah += ( top ? _spacing[1] : 0.0 );
					y += ah;
					h -= ah;
					top = true;
					break;

				// Fill the space at the top left
				case base::alignment::TOP_LEFT:
					a->set( { x1() + x, y1() + y }, { aw, ah } );
					aw += ( left ? _spacing[0] : 0.0 );
					ah += ( top ? _spacing[1] : 0.0 );
					x += aw;
					y += ah;
					w -= aw;
					h -= ah;
					top = true;
					left = true;
					break;

				case base::alignment::TOP_RIGHT:
					a->set( { x1() + x, y1() + y + h - ah }, { aw, ah } );
					aw += ( right ? _spacing[0] : 0.0 );
					ah += ( top ? _spacing[1] : 0.0 );
					y += ah;
					w -= aw;
					h -= ah;
					top = true;
					right = true;
					break;

				case base::alignment::BOTTOM:
					a->set( { x1() + x, y1() + y + h - ah }, { w, ah } );
					ah += ( bottom ? _spacing[1] : 0.0 );
					h -= ah;
					bottom = true;
					break;

				case base::alignment::BOTTOM_RIGHT:
					a->set( { x1() + x + w - aw, y1() + y + h - ah }, { aw, ah } );
					aw += ( right ? _spacing[0] : 0.0 );
					ah += ( bottom ? _spacing[1] : 0.0 );
					w -= aw;
					h -= ah;
					bottom = true;
					right = true;
					break;

				case base::alignment::BOTTOM_LEFT:
					a->set( { x1() + x, y1() + y + h - ah }, { aw, ah } );
					aw += ( left ? _spacing[0] : 0.0 );
					ah += ( bottom ? _spacing[1] : 0.0 );
					x += aw;
					w -= aw;
					h -= ah;
					bottom = true;
					left = true;
					break;

				case base::alignment::RIGHT:
					a->set( { x1() + x + w - aw, y1() + y }, { aw, h } );
					aw += ( right ? _spacing[0] : 0.0 );
					w -= aw;
					right = true;
					break;

				case base::alignment::LEFT:
					a->set( { x1() + x, y1() + y }, { aw, h } );
					aw += ( right ? _spacing[0] : 0.0 );
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

