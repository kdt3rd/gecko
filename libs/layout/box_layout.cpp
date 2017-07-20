//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "box_layout.h"
#include <base/contract.h>

namespace layout
{

////////////////////////////////////////

box_layout::box_layout( base::alignment direction )
	: _align( direction )
{
	precondition( direction != base::alignment::CENTER, "invalid direction for box layout" );
}

////////////////////////////////////////

void box_layout::compute_bounds( void )
{
	double minw = 0.0;
	double minh = 0.0;
	double maxw = 0.0;
	double maxh = 0.0;
	switch ( _align )
	{
		case base::alignment::LEFT:
		case base::alignment::RIGHT:
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
			minw += _spacing[0] * ( _areas.size() - 1 );
			maxw += _spacing[0] * ( _areas.size() - 1 );
			break;

		case base::alignment::TOP:
		case base::alignment::BOTTOM:
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
			minh += _spacing[1] * ( _areas.size() - 1 );
			maxh += _spacing[1] * ( _areas.size() - 1 );
			break;

		case base::alignment::TOP_LEFT:
		case base::alignment::TOP_RIGHT:
		case base::alignment::BOTTOM_LEFT:
		case base::alignment::BOTTOM_RIGHT:
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
			minw += _spacing[0] * ( _areas.size() - 1 );
			maxw += _spacing[0] * ( _areas.size() - 1 );
			minh += _spacing[1] * ( _areas.size() - 1 );
			maxh += _spacing[1] * ( _areas.size() - 1 );
			break;

		case base::alignment::CENTER:
			throw_runtime( "invalid direction for box layout" );
	}
	minw += _pad[0] + _pad[1];
	minh += _pad[2] + _pad[3];
	maxw += _pad[0] + _pad[1];
	maxh += _pad[2] + _pad[3];

	set_minimum( minw, minh );
	set_maximum( maxw, maxh );
}

////////////////////////////////////////

void box_layout::compute_layout( void )
{
	// Clean up areas that have been deleted.
	_areas.remove_if( []( const std::weak_ptr<area> &a ) { return a.expired(); } );

	double x = 0.0, y = 0.0;
	double dx = 0.0, dy = 0.0;

	switch ( _align )
	{
		case base::alignment::LEFT:
			x = width() - _pad[1];
			y = _pad[2];
			dx = -1.0;
			dy = 0.0;
			break;

		case base::alignment::RIGHT:
			x = _pad[0];
			y = _pad[2];
			dx = 1.0;
			dy = 0.0;
			break;

		case base::alignment::TOP:
			x = _pad[0];
			y = height() - _pad[3];
			dx = 0.0;
			dy = -1.0;
			break;

		case base::alignment::BOTTOM:
			x = _pad[0];
			y = _pad[2];
			dx = 0.0;
			dy = 1.0;
			break;

		case base::alignment::TOP_LEFT:
			x = width() - _pad[1];
			y = height() - _pad[3];
			dx = -1.0;
			dy = -1.0;
			break;

		case base::alignment::TOP_RIGHT:
			x = _pad[0];
			y = height() - _pad[3];
			dx = 1.0;
			dy = -1.0;
			break;

		case base::alignment::BOTTOM_LEFT:
			x = width() - _pad[1];
			y = _pad[2];
			dx = -1.0;
			dy = 1.0;
			break;

		case base::alignment::BOTTOM_RIGHT:
			x = _pad[0];
			y = _pad[2];
			dx = 1.0;
			dy = 1.0;
			break;

		case base::alignment::CENTER:
			throw_runtime( "invalid direction for box layout" );
	}

	for ( auto &wa: _areas )
	{
		auto a = wa.lock();
		if ( a )
			a->set_size( a->minimum_size() );
	}

	double w = std::max( 0.0, width() - _pad[0] - _pad[1] );
	double h = std::max( 0.0, height() - _pad[2] - _pad[3] );

	if ( dx != 0.0 )
	{
		expand_width( _areas, width() - minimum_width() );
		w = 0.0;
	}
	if ( dy != 0.0 )
	{
		expand_height( _areas, height() - minimum_height() );
		h = 0.0;
	}

	for ( auto &wa: _areas )
	{
		auto a = wa.lock();
		if ( a )
		{
			double aw = a->width();
			double ah = a->height();
			a->set( { x, y }, { aw * dx + w, ah * dy + h } );
			x += ( aw + _spacing[0] ) * dx;
			y += ( ah + _spacing[1] ) * dy;
			a->compute_layout();
		}
	}
}

////////////////////////////////////////

}

