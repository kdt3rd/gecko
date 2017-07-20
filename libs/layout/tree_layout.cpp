
#include "tree_layout.h"

namespace layout
{

////////////////////////////////////////

void tree_layout::compute_bounds( void )
{
	auto g = _groove.lock();
	auto t = _title.lock();
	auto c = _content.lock();

	double minw = 0.0;
	double minh = 0.0;
	double maxw = 0.0;
	double maxh = 0.0;

	if ( t )
	{
		t->compute_bounds();
		minw = std::max( minw, t->minimum_width() );
		minh += t->minimum_height();
		maxw = std::max( maxw, t->maximum_width() );
		maxh += t->maximum_height();
	}
	if ( c )
	{
		c->compute_bounds();
		minw = std::max( minw, c->minimum_width() );
		minh += c->minimum_height();
		maxw = std::max( maxw, c->maximum_width() );
		maxh += c->maximum_height();
	}
	if ( g )
	{
		g->compute_bounds();
		minw += std::max( _indent, g->minimum_width() );
		minh = std::max( minh, g->minimum_height() );
		maxw += std::max( _indent, g->maximum_width() );
		maxh = std::max( maxh, g->maximum_height() );
	}

	minw += _pad[0] + _pad[1] + _spacing[0];
	minh += _pad[2] + _pad[3] + _spacing[1];
	maxw += _pad[0] + _pad[1] + _spacing[0];
	maxh += _pad[2] + _pad[3] + _spacing[1];

	set_minimum( minw, minh );
	set_maximum( maxw, maxh );
}

////////////////////////////////////////

void tree_layout::compute_layout( void )
{
	auto g = _groove.lock();
	auto t = _title.lock();
	auto c = _content.lock();


	double iw = _indent;
	if ( g )
		iw = std::max( _indent, g->minimum_width() );

	double cw = std::max( 0.0, width() - _pad[0] - _pad[1] - _spacing[0] - iw );
	double th = 0.0;
	if ( t )
		th = t->minimum_height();

	double ch = height() - _pad[2] - _pad[3] - _spacing[1] - th;

	double x = _pad[0];
	double y = _pad[2];
	if ( g )
	{
		g->set( { x, y }, { iw, th + _spacing[1] + ch } );
		g->compute_layout();
	}
	x += iw + _spacing[0];
	if ( t )
	{
		t->set( { x, y }, { cw, th } );
		t->compute_layout();
	}
	y += th + _spacing[1];
	if ( c )
	{
		c->set( { x, y }, { cw, ch } );
		c->compute_layout();
	}
}

////////////////////////////////////////

}
