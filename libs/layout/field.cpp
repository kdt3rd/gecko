// Copyright (c) 2017 Ian Godin
// SPDX-License-Identifier: MIT

#include "field.h"
#include <base/contract.h>

namespace layout
{

////////////////////////////////////////

field::field( const std::shared_ptr<area> &l, const std::shared_ptr<area> &e )
	: _label( l ), _field( e )
{
}

////////////////////////////////////////

coord field::field_minimum_width( void )
{
	auto f = _field.lock();
	if ( f )
	{
		f->compute_bounds();
		return f->minimum_width();
	}
	return min_coord();
}

////////////////////////////////////////

void field::compute_bounds( void )
{
	auto l = _label.lock();
	auto f = _field.lock();

	coord minw = _pad[0] + _pad[1];
	coord minh = min_coord();
	coord maxw = _pad[0] + _pad[1];
	coord maxh = min_coord();


	if ( l )
	{
		minw += l->minimum_width();
		minh += l->minimum_height();
		maxw += l->maximum_width();
		maxh += l->maximum_height();
	}

	if ( l && f )
		minw += _spacing[0];

	if ( f )
	{
		coord w = std::max( f->minimum_width(), _width );
		minw += w;
		minh = std::max( minh, f->minimum_height() );
		maxw += f->maximum_width();
		maxh = std::max( maxh, f->maximum_height() );
	}

	minh += _pad[2] + _pad[3];
	maxh += _pad[2] + _pad[3];

	set_minimum( minw, minh );
	set_maximum( maxw, minw );
}

////////////////////////////////////////

void field::compute_layout( void )
{
	auto l = _label.lock();
	auto f = _field.lock();
	coord fw = _width;
	coord w = width() - _pad[0] - _pad[1];
	coord h = height() - _pad[2] - _pad[3];
	coord x = _pad[0];
	if ( f )
	{
		fw = std::max( fw, f->minimum_width() );
		w -= _spacing[0];
	}
	w -= fw;
	if ( l )
	{
		l->set( { x1() + x, y1() + _pad[2] }, { w, h } );
		l->compute_layout();
		x += w + _spacing[0];
	}
	if ( f )
	{
		f->set( { x1() + x, y1() + _pad[2] }, { fw, h } );
		f->compute_layout();
	}
}

////////////////////////////////////////

}

