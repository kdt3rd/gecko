//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "grid.h"
#include <base/contract.h>

namespace layout
{

////////////////////////////////////////

size_t grid::add_columns( size_t n, coord flex, int32_t pri )
{
	size_t result = _cols.size();
	for ( size_t i = 0; i < n; ++i )
	{
		auto a = std::make_shared<area>();
		a->set_expansion_flex( flex );
		a->set_expansion_priority( pri );
		_cols.push_back( std::move( a ) );
	}
	return result;
}

////////////////////////////////////////

size_t grid::add_rows( size_t n, coord flex, int32_t pri )
{
	size_t result = _rows.size();
	for ( size_t i = 0; i < n; ++i )
	{
		auto a = std::make_shared<area>();
		a->set_expansion_flex( flex );
		a->set_expansion_priority( pri );
		_rows.push_back( std::move( a ) );
	}
	return result;
}

////////////////////////////////////////

void grid::add( const std::shared_ptr<area> &a, size_t x, size_t y, size_t w, size_t h )
{
	_areas.emplace_back( a, x, y, w, h );
}

////////////////////////////////////////

void grid::compute_bounds( void )
{
	// TODO Compute maximum size for grid.

	// Reset columns and rows to 0.
	for ( auto &c: _cols )
		c->set_extent( 0, 0 );
	for ( auto &r: _rows )
		r->set_extent( 0, 0 );

	std::list<std::shared_ptr<area>> tmp;
	for ( auto &c: _areas )
	{
		auto a = c._area.lock();
		if ( a )
		{
			a->compute_bounds();

			// Handle width
			coord w = min_coord();
			tmp.clear();
			for ( size_t x = 0; x < c._w; ++x )
			{
				auto col = _cols.at( c._x + x );
				w += col->width();
				tmp.push_back( std::move( col ) );
			}
			coord extra = a->minimum_width() - w;
			expand_width( tmp, extra );

			// Handle height
			coord h = min_coord();
			tmp.clear();
			for ( size_t y = 0; y < c._h; ++y )
			{
				auto row = _rows.at( c._y + y );
				h += row->height();
				tmp.push_back( std::move( row ) );
			}
			extra = a->minimum_height() - h;
			expand_height( tmp, extra );
		}
	}

	coord minw = min_coord();
	if ( !_cols.empty() )
	{
		for ( auto &c: _cols )
		{
			c->set_minimum_width( c->width() );
			minw += c->minimum_width();
		}
		minw += ( _cols.size() - 1 ) * _spacing[0];
	}
	minw += _pad[0] + _pad[1];

	coord minh = min_coord();
	if ( !_rows.empty() )
	{
		for ( auto &r: _rows )
		{
			r->set_minimum_height( r->height() );
			minh += r->minimum_height();
		}
		minh += ( _rows.size() - 1 ) * _spacing[1];
	}
	minh += _pad[2] + _pad[3];

	set_minimum( minw, minh );
}

////////////////////////////////////////

void grid::compute_layout( void )
{
	// Set columns to minimum size and add any extra space
	std::list<std::shared_ptr<area>> tmp;
	for ( auto &c: _cols )
	{
		c->set_size( c->minimum_size() );
		tmp.push_back( c );
	}
	expand_width( tmp, width() - minimum_width() );

	// Now layout columns left to right
	coord x = _pad[0];
	for ( auto &c: _cols )
	{
		c->set_position( { x, 0 } );
		x += c->width() + _spacing[0];
	}

	// Set rows to minimum size and add any extra space
	tmp.clear();
	for ( auto &r: _rows )
	{
		r->set_size( r->minimum_size() );
		tmp.push_back( r );
	}
	expand_height( tmp, height() - minimum_height() );

	// Now layout rows top to bottom
	coord y = _pad[2];
	for ( auto &r: _rows )
	{
		r->set_position( { 0, y } );
		y += r->height() + _spacing[1];
	}

	// Finally layout out areas according to the cell they occupy
	for ( auto &c: _areas )
	{
		auto a = c._area.lock();
		if ( a )
		{
			auto l = _cols.at( c._x );
			auto r = _cols.at( c._x + c._w - 1 );
			auto t = _rows.at( c._y );
			auto b = _rows.at( c._y + c._h - 1 );
			a->set_x1( x1() + l->x1() );
			a->set_y1( y1() + t->y1() );
			a->set_x2( x1() + r->x2() + 1 );
			a->set_y2( y1() + b->y2() + 1 );
			a->compute_layout();
		}
	}
}

////////////////////////////////////////

}

