//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "border_layout.h"

namespace layout
{

////////////////////////////////////////

border_layout::border_layout( void )
{
}

////////////////////////////////////////

void border_layout::compute_minimum( void )
{
	auto t = _top.lock();
	auto b = _bottom.lock();
	auto l = _left.lock();
	auto r = _right.lock();
	auto c = _center.lock();
	if ( t )
		t->compute_minimum();
	if ( b )
		b->compute_minimum();
	if ( l )
		l->compute_minimum();
	if ( r )
		r->compute_minimum();
	if ( c )
		c->compute_minimum();

	double w = 0.0;
	if ( l )
		w += l->minimum_width();
	if ( c )
		w += c->minimum_width();
	if ( r )
		w += r->minimum_width();
	if ( t )
		w = std::max( w, t->minimum_width() );
	if ( b )
		w = std::max( w, b->minimum_width() );

	double h = 0.0;
	if ( l )
		h = std::max( h, l->minimum_height() );
	if ( c )
		h = std::max( h, c->minimum_height() );
	if ( r )
		h = std::max( h, r->minimum_height() );
	if ( t )
		h += t->minimum_height();
	if ( b )
		h += b->minimum_height();

	set_minimum( w + _pad[0] + _pad[1] + _spacing[0] * 2.0, h + _pad[2] + _pad[3] + _spacing[1] * 2.0 );
}

////////////////////////////////////////

void border_layout::compute_layout( void )
{
	auto t = _top.lock();
	auto b = _bottom.lock();
	auto l = _left.lock();
	auto r = _right.lock();
	auto c = _center.lock();

	// Calculate center width
	double w1 = l ? l->minimum_width() : 0.0;
	double w3 = r ? r->minimum_width() : 0.0;
	double w2 = std::max( 0.0, width() - w1 - w3 - _pad[0] - _pad[1] - _spacing[0] * 2.0 );
	if ( c )
		w2 = std::max( w2, c->minimum_width() );

	double h1 = t ? t->minimum_height() : 0.0;
	double h3 = b ? b->minimum_height() : 0.0;
	double h2 = std::max( 0.0, height() - h1 - h3 - _pad[1] - _pad[2] - _spacing[1] * 2.0 );
	if ( c )
		h2 = std::max( h2, c->minimum_height() );
	if ( l )
		h2 = std::max( h2, l->minimum_height() );
	if ( r )
		h2 = std::max( h2, r->minimum_height() );

	// Set the position and size
	if ( l )
	{
		l->set( { _pad[0], _pad[2] + h1 + _spacing[1] }, { w1, h2 } );
		l->compute_layout();
	}
	if ( r )
	{
		r->set( { _pad[0] + w1 + _spacing[0] * 2.0 + w2, _pad[2] + h1 + _spacing[1] }, { w3, h2 } );
		r->compute_layout();
	}
	if ( t )
	{
		t->set( { _pad[0], _pad[2] }, { w1 + w2 + w3 + _spacing[0] * 2.0, h1 } );
		t->compute_layout();
	}
	if ( b )
	{
		b->set( { _pad[0], _pad[2] + h1 + _spacing[1] * 2.0 + h2 }, { w1 + w2 + w3 + _spacing[0] * 2.0, h3 } );
		b->compute_layout();
	}
	if ( c )
	{
		c->set( { _pad[0] + w1 + _spacing[0], _pad[2] + h1 + _spacing[1] }, { w2, h2 } );
		c->compute_layout();
	}
}

////////////////////////////////////////

}

