
#include "grid.h"

namespace layout
{

////////////////////////////////////////

grid::grid( size_t w, size_t h )
	: _hspacing( "spacing.h" ), _vspacing( "spacing.v" ), _hpadding( "padding.h" ), _vpadding( "padding.v" )
{
	create_tabs( w, h );
}

////////////////////////////////////////

void grid::add( area &a, size_t x, size_t y, size_t w, size_t h )
{
	precondition( w > 0, "invalid grid cell width ({0})", w );
	precondition( h > 0, "invalid grid cell height ({0})", h );
	precondition( w < _htabs.size(), "invalid grid cell width ({0})", w );
	precondition( h < _vtabs.size(), "invalid grid cell height ({0})", h );
	precondition( x + w < _htabs.size(), "invalid grid cell location ({0}+{1})", x, w );
	precondition( y + h < _vtabs.size(), "invalid grid cell location ({0}+{1})", y, h );

	add_constraint( a.left() == _htabs[x] + _hspacing );
	add_constraint( a.right() == _htabs[x+w] - _hspacing );
	add_constraint( a.top() == _vtabs[y] + _vspacing );
	add_constraint( a.bottom() == _vtabs[y+h] - _vspacing );

	add_constraint( medium || a.width() >= a.minimum_width() );
	add_constraint( medium || a.height() >= a.minimum_height() );
}

////////////////////////////////////////

void grid::reset( void )
{
	layout::reset();
	create_tabs( _htabs.size(), _vtabs.size() );
}

////////////////////////////////////////

void grid::set_spacing( double h, double v )
{
	_default_hspacing = h;
	_default_vspacing = v;
	_solver.suggest( _hspacing, _default_hspacing );
	_solver.suggest( _vspacing, _default_vspacing );
}

////////////////////////////////////////

void grid::set_padding( double h, double v )
{
	_default_hpadding = h;
	_default_vpadding = v;
	_solver.suggest( _hpadding, _default_hpadding );
	_solver.suggest( _vpadding, _default_vpadding );
}

////////////////////////////////////////

void grid::create_tabs( size_t w, size_t h )
{
	precondition( w > 0, "invalid grid width" );
	precondition( h > 0, "invalid grid height" );

	_htabs.clear();
	_vtabs.clear();

	variable left( "grid.l" );
	variable top( "grid.t" );
	variable right( "grid.r" );
	variable bottom( "grid.b" );

	_htabs.push_back( left );
	_solver.add_constraint( left == _left + _hpadding );
	_vtabs.push_back( top );
	_solver.add_constraint( top == _top + _vpadding );

	for ( size_t x = 1; x < w; ++x )
		_htabs.emplace_back( "htab" + base::to_string( x ) );

	for ( size_t y = 1; y < h; ++y )
		_vtabs.emplace_back( "vtab" + base::to_string( y ) );

	_htabs.push_back( right );
	_solver.add_constraint( right == _right - _hpadding );
	_vtabs.push_back( bottom );
	_solver.add_constraint( bottom == _bottom - _vpadding );

	// Constrain the tabs to be consecutive.
	for ( size_t x = 1; x < _htabs.size(); ++x )
		add_constraint( _htabs[x-1] + _hspacing * 2 <= _htabs[x] );
	for ( size_t y = 1; y < _vtabs.size(); ++y )
		add_constraint( _vtabs[y-1] + _vspacing * 2 <= _vtabs[y] );

	_solver.add_variable( _hspacing );
	_solver.suggest( _hspacing, _default_hspacing );
	_solver.add_variable( _vspacing );
	_solver.suggest( _vspacing, _default_vspacing );
	_solver.add_variable( _hpadding );
	_solver.suggest( _hpadding, _default_hpadding );
	_solver.add_variable( _vpadding );
	_solver.suggest( _vpadding, _default_vpadding );
}

////////////////////////////////////////

}
