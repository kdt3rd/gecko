//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "packing_layout.h"

namespace layout
{

////////////////////////////////////////

packing_layout::packing_layout( void )
{
}

////////////////////////////////////////

void packing_layout::compute_bounds( void )
{
	double minw = _pad[0] + _pad[1];
	double minh = _pad[2] + _pad[3];
	double maxw = _pad[0] + _pad[1];
	double maxh = _pad[2] + _pad[3];

	bool top = false;
	bool bottom = false;
	bool left = false;
	bool right = false;

	for ( auto &s: _areas )
	{
		auto a = s._area.lock();
		if ( a )
		{
			a->compute_bounds();
			switch ( s._align )
			{
				case base::alignment::CENTER:
					minw += a->minimum_width() + ( left ? _spacing[0] : 0.0 ) + ( right ? _spacing[0] : 0.0 );
					minh += a->minimum_height() + ( top ? _spacing[1] : 0.0 ) + ( bottom ? _spacing[1] : 0.0 );
					maxw += a->maximum_width() + ( left ? _spacing[0] : 0.0 ) + ( right ? _spacing[0] : 0.0 );
					maxh += a->maximum_height() + ( top ? _spacing[1] : 0.0 ) + ( bottom ? _spacing[1] : 0.0 );
					top = true;
					bottom = true;
					right = true;
					left = true;
					break;

				case base::alignment::TOP:
					minh += a->minimum_height() + ( top ? _spacing[1] : 0.0 );
					maxh += a->maximum_height() + ( top ? _spacing[1] : 0.0 );
					minw = std::max( minw, a->minimum_width() );
					maxw = std::max( maxw, a->maximum_width() );
					top = true;
					break;

				case base::alignment::TOP_LEFT:
					minw += a->minimum_width() + ( left ? _spacing[0] : 0.0 );
					minh += a->minimum_height() + ( top ? _spacing[1] : 0.0 );
					maxw += a->maximum_width() + ( left ? _spacing[0] : 0.0 );
					maxh += a->maximum_height() + ( top ? _spacing[1] : 0.0 );
					top = true;
					left = true;
					break;

				case base::alignment::TOP_RIGHT:
					minw += a->minimum_width() + ( right ? _spacing[0] : 0.0 );
					minh += a->minimum_height() + ( top ? _spacing[1] : 0.0 );
					maxw += a->maximum_width() + ( right ? _spacing[0] : 0.0 );
					maxh += a->maximum_height() + ( top ? _spacing[1] : 0.0 );
					top = true;
					right = true;
					break;

				case base::alignment::BOTTOM:
					minh += a->minimum_height() + ( bottom ? _spacing[1] : 0.0 );
					minw = std::max( minw, a->minimum_width() );
					maxh += a->maximum_height() + ( bottom ? _spacing[1] : 0.0 );
					maxw = std::max( maxw, a->maximum_width() );
					bottom = true;
					break;

				case base::alignment::BOTTOM_RIGHT:
					minw += a->minimum_width() + ( right ? _spacing[0] : 0.0 );
					minh += a->minimum_height() + ( bottom ? _spacing[1] : 0.0 );
					maxw += a->maximum_width() + ( right ? _spacing[0] : 0.0 );
					maxh += a->maximum_height() + ( bottom ? _spacing[1] : 0.0 );
					bottom = true;
					right = true;
					break;

				case base::alignment::BOTTOM_LEFT:
					minw += a->minimum_width() + ( left ? _spacing[0] : 0.0 );
					minh += a->minimum_height() + ( bottom ? _spacing[1] : 0.0 );
					maxw += a->maximum_width() + ( left ? _spacing[0] : 0.0 );
					maxh += a->maximum_height() + ( bottom ? _spacing[1] : 0.0 );
					bottom = true;
					left = true;
					break;

				case base::alignment::RIGHT:
					minw += a->minimum_width() + ( right ? _spacing[0] : 0.0 );
					minh = std::max( minh, a->minimum_height() );
					maxw += a->maximum_width() + ( right ? _spacing[0] : 0.0 );
					maxh = std::max( maxh, a->maximum_height() );
					right = true;
					break;

				case base::alignment::LEFT:
					minw += a->minimum_width() + ( left ? _spacing[0] : 0.0 );
					minh = std::max( minh, a->minimum_height() );
					maxw += a->maximum_width() + ( left ? _spacing[0] : 0.0 );
					maxh = std::max( maxh, a->maximum_height() );
					right = true;
					break;
			}
		}
	}

	set_minimum( minw, minh );
	set_maximum( maxw, maxh );
}

////////////////////////////////////////

void packing_layout::compute_layout( void )
{
	// Clean up areas that have been deleted.
	_areas.remove_if( []( const section &s ) { return s._area.expired(); } );

	double x = _pad[0];
	double y = _pad[2];
	double w = std::max( width() - _pad[0] - _pad[1], minimum_width() );
	double h = std::max( height() - _pad[2] - _pad[3], minimum_height() );

	for ( auto &s: _areas )
	{
		auto a = s._area.lock();
		if ( a )
		{
			if ( w <= 0.0 || h <= 0.0 )
			{
				a->set( { x, y }, { w, h } );
				a->compute_layout();
				continue;
			}

			float aw = a->minimum_width();
			float ah = a->minimum_height();
			bool top = false;
			bool bottom = false;
			bool left = false;
			bool right = false;

			switch ( s._align )
			{
				// Fill the last remaining space
				case base::alignment::CENTER:
					a->set( { x, y }, { w, h } );
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
					a->set( { x, y }, { w, ah } );
					ah += ( top ? _spacing[1] : 0.0 );
					y += ah;
					h -= ah;
					top = true;
					break;

				// Fill the space at the top left
				case base::alignment::TOP_LEFT:
					a->set( { x, y }, { aw, ah } );
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
					a->set( { x, y + h - ah }, { aw, ah } );
					aw += ( right ? _spacing[0] : 0.0 );
					ah += ( top ? _spacing[1] : 0.0 );
					y += ah;
					w -= aw;
					h -= ah;
					top = true;
					right = true;
					break;

				case base::alignment::BOTTOM:
					a->set( { x, y + h - ah }, { w, ah } );
					ah += ( bottom ? _spacing[1] : 0.0 );
					h -= ah;
					bottom = true;
					break;

				case base::alignment::BOTTOM_RIGHT:
					a->set( { x + w - aw, y + h - ah }, { aw, ah } );
					aw += ( right ? _spacing[0] : 0.0 );
					ah += ( bottom ? _spacing[1] : 0.0 );
					w -= aw;
					h -= ah;
					bottom = true;
					right = true;
					break;

				case base::alignment::BOTTOM_LEFT:
					a->set( { x, y + h - ah }, { aw, ah } );
					aw += ( left ? _spacing[0] : 0.0 );
					ah += ( bottom ? _spacing[1] : 0.0 );
					x += aw;
					w -= aw;
					h -= ah;
					bottom = true;
					left = true;
					break;

				case base::alignment::RIGHT:
					a->set( { x + w - aw, y }, { aw, h } );
					aw += ( right ? _spacing[0] : 0.0 );
					w -= aw;
					right = true;
					break;

				case base::alignment::LEFT:
					a->set( { x, y }, { aw, h } );
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

