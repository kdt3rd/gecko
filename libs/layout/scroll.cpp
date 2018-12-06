//
// Copyright (c) 2017 Ian Godin
// SPDX-License-Identifier: MIT
//

#include "scroll.h"
#include <base/contract.h>

namespace layout
{

////////////////////////////////////////

void scroll::compute_bounds( void )
{
	std::shared_ptr<area> m = _main.lock();
	std::shared_ptr<area> h = _hscroll.lock();
	std::shared_ptr<area> v = _vscroll.lock();
	std::shared_ptr<area> c = _corner.lock();

	coord minw = min_coord();
	coord minh = min_coord();

	if ( v )
		minw += v->minimum_width() + _spacing[0];

	if ( h )
		minh += h->minimum_height() + _spacing[1];

	if ( c )
	{
		minw = std::max( minw, c->minimum_width() + _spacing[0] );
		minh = std::max( minh, c->minimum_height() + _spacing[1] );
	}

	if ( m )
	{
		minw += std::max( m->minimum_width(), minw - _spacing[0] );
		minh += std::max( m->minimum_height(), minh - _spacing[1] );
	}

	minw += _pad[0] + _pad[1];
	minh += _pad[2] + _pad[3];

	set_minimum( minw, minh );

	std::shared_ptr<area> s = _sublayout.lock();
	if ( s )
		s->compute_bounds();
}

////////////////////////////////////////

void scroll::compute_layout( void )
{
	std::shared_ptr<area> m = _main.lock();
	std::shared_ptr<area> h = _hscroll.lock();
	std::shared_ptr<area> v = _vscroll.lock();
	std::shared_ptr<area> c = _corner.lock();

	coord vw = min_coord();
	coord vh = min_coord();

	if ( c )
	{
		vw = c->minimum_width();
		vh = c->minimum_height();
	}

	if ( v )
		vw = std::max( vw, v->minimum_width() );
	else
		vw = min_coord();

	if ( h )
		vh = std::max( vh, h->minimum_height() );
	else
		vh = min_coord();

	coord mw = std::max( min_coord(), width() - _pad[0] - _pad[1] - ( vw > min_coord() ? vw + _spacing[0] : min_coord() ) );
	coord mh = std::max( min_coord(), height() - _pad[2] - _pad[3] - ( vh > min_coord() ? vh + _spacing[1] : min_coord() ) );

	if ( m )
	{
		m->set_position( { x() + _pad[0], y() + _pad[2] } );
		m->set_size( mw, mh );
		m->compute_layout();
	}

	if ( v )
	{
		v->set_position( { x() + _pad[0] + mw + _spacing[0], y() + _pad[2] } );
		v->set_size( vw, mh );
		v->compute_layout();
	}

	if ( h )
	{
		h->set_position( { x() + _pad[0], y() + _pad[2] + mh + _spacing[1] } );
		h->set_size( mw, vh );
		v->compute_layout();
	}

	if ( c )
	{
		c->set_position( { x() + _pad[0] + mw + _spacing[0], y() + _pad[2] + mh + _spacing[1] } );
		c->set_size( vw, vh );
		c->compute_layout();
	}

	std::shared_ptr<area> s = _sublayout.lock();
	if ( s )
	{
		s->set_size( s->minimum_size() );
		s->compute_layout();
	}
}

////////////////////////////////////////

}

