//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "field_layout.h"
#include <base/contract.h>

namespace layout
{

////////////////////////////////////////

field_layout::field_layout( const std::shared_ptr<area> &l, const std::shared_ptr<area> &e )
	: _label( l ), _field( e )
{
}

////////////////////////////////////////

double field_layout::field_minimum_width( void )
{
	auto f = _field.lock();
	if ( f )
	{
		f->compute_bounds();
		return f->minimum_width();
	}
	return 0.0;
}

////////////////////////////////////////

void field_layout::compute_bounds( void )
{
	auto l = _label.lock();
	auto f = _field.lock();

	double minw = _pad[0] + _pad[1];
	double minh = 0.0;
	double maxw = _pad[0] + _pad[1];
	double maxh = 0.0;

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
		double w = std::max( f->minimum_width(), _width );
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

void field_layout::compute_layout( void )
{
	auto l = _label.lock();
	auto f = _field.lock();
	double fw = _width;
	double w = width() - _pad[0] - _pad[1];
	double h = height() - _pad[2] - _pad[3];
	double x = _pad[0];
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

