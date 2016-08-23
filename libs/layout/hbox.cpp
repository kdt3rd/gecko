
#include "hbox.h"

namespace layout
{

////////////////////////////////////////

hbox::hbox( void )
	: _spacing( "spacing" ), _padding( "padding" )
{
	setup();
}

////////////////////////////////////////

void hbox::add( area &a, double fraction )
{
	precondition( !_init, "cannot add new areas after update" );

	add_constraint( a.top() == _top + _padding + _spacing );
	add_constraint( a.bottom() == _bottom - _padding - _spacing );

//	if ( fraction > 1e-8 )
//		add_constraint( weak || a.width() >= width() * fraction );

	add_constraint( a.left() == _current + _spacing );
	add_constraint( a.width() >= a.minimum_width() );

	_current = a.right() + _spacing;
}

////////////////////////////////////////

void hbox::setup( void )
{
	_init = false;

	_solver.add_variable( _spacing );
	_solver.add_variable( _padding );
	_solver.suggest( _spacing, _default_spacing );
	_solver.suggest( _padding, _default_padding );

	_current = _left + _padding;
}

////////////////////////////////////////

void hbox::update( void )
{
	if ( !_init )
	{
		_solver.update_variables();
		std::cout << "CURRENT = " << _current.value() << std::endl;
		std::cout << "OTHER = " << ( _right - _padding ).value() << std::endl;
		_solver.add_constraint( _current == _right - _padding );
		_init = true;
	}
	layout::update();
}

////////////////////////////////////////

void hbox::reset( void )
{
	layout::reset();
	setup();
}

////////////////////////////////////////

}
