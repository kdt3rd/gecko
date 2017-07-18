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

void packing_layout::compute_minimum( void )
{
	double w = _pad[0] + _pad[1];
	double h = _pad[2] + _pad[3];

	bool top = false;
	bool bottom = false;
	bool left = false;
	bool right = false;

	for ( auto &s: _areas )
	{
		auto a = s._area.lock();
		if ( a )
		{
			a->compute_minimum();
			switch ( s._align )
			{
				case base::alignment::CENTER:
					w += a->minimum_width() + ( left ? _spacing[0] : 0.0 ) + ( right ? _spacing[0] : 0.0 );
					h += a->minimum_height() + ( top ? _spacing[1] : 0.0 ) + ( bottom ? _spacing[1] : 0.0 );
					top = true;
					bottom = true;
					right = true;
					left = true;
					break;

				case base::alignment::TOP:
					h += a->minimum_height() + ( top ? _spacing[1] : 0.0 );
					w = std::max( w, a->minimum_width() );
					top = true;
					break;

				case base::alignment::TOP_LEFT:
					w += a->minimum_width() + ( left ? _spacing[0] : 0.0 );
					h += a->minimum_height() + ( top ? _spacing[1] : 0.0 );
					top = true;
					left = true;
					break;

				case base::alignment::TOP_RIGHT:
					w += a->minimum_width() + ( right ? _spacing[0] : 0.0 );
					h += a->minimum_height() + ( top ? _spacing[1] : 0.0 );
					top = true;
					right = true;
					break;

				case base::alignment::BOTTOM:
					h += a->minimum_height() + ( bottom ? _spacing[1] : 0.0 );
					w = std::max( w, a->minimum_width() );
					bottom = true;
					break;

				case base::alignment::BOTTOM_RIGHT:
					w += a->minimum_width() + ( right ? _spacing[0] : 0.0 );
					h += a->minimum_height() + ( bottom ? _spacing[1] : 0.0 );
					bottom = true;
					right = true;
					break;

				case base::alignment::BOTTOM_LEFT:
					w += a->minimum_width() + ( left ? _spacing[0] : 0.0 );
					h += a->minimum_height() + ( bottom ? _spacing[1] : 0.0 );
					bottom = true;
					left = true;
					break;

				case base::alignment::RIGHT:
					w += a->minimum_width() + ( right ? _spacing[0] : 0.0 );
					h = std::max( h, a->minimum_height() );
					right = true;
					break;

				case base::alignment::LEFT:
					w += a->minimum_width() + ( left ? _spacing[0] : 0.0 );
					h = std::max( h, a->minimum_height() );
					right = true;
					break;
			}
		}
	}

	set_minimum( w, h );
}

////////////////////////////////////////

void packing_layout::compute_layout( void )
{
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

