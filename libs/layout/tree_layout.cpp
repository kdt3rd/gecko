
#include "tree_layout.h"
#include <base/format.h>

namespace layout
{

////////////////////////////////////////

void tree_layout::compute_bounds( void )
{
	auto g = _groove.lock();
	auto t = _title.lock();

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
	for ( auto &w: _areas )
	{
		auto a = w.lock();
		if ( a )
		{
			a->compute_bounds();
			if ( std::dynamic_pointer_cast<tree_layout>( a ) )
				minw = std::max( minw, a->minimum_width() );
			else
				minw = std::max( minw, a->minimum_width() + _indent + _spacing[0] );
			minh += a->minimum_height();
			if ( std::dynamic_pointer_cast<tree_layout>( a ) )
				maxw = std::max( maxw, a->maximum_width() );
			else
				maxw = std::max( maxw, a->maximum_width() + _indent + _spacing[0] );
			maxh += a->maximum_height();
		}
	}
	if ( !_areas.empty() )
	{
		minh += ( _areas.size() - 1 ) * _spacing[1];
		maxh += ( _areas.size() - 1 ) * _spacing[1];
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
	for ( auto &w: _areas )
	{
		auto a = w.lock();
		if ( a )
		{
			if ( std::dynamic_pointer_cast<tree_layout>( a ) )
			{
				a->set( { x, y }, { cw, a->minimum_height() } );
				a->compute_layout();
			}
			else
			{
				a->set( { x + _indent + _spacing[0], y }, { cw - _indent - _spacing[0], a->minimum_height() } );
				a->compute_layout();
			}
			y += a->minimum_height() + _spacing[1];
		}
	}
}

////////////////////////////////////////

}
