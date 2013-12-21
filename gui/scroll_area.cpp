
#include "scroll_area.h"

namespace gui
{

////////////////////////////////////////

scroll_area::scroll_area( void )
{
}

////////////////////////////////////////

scroll_area::scroll_area( double w, double h )
{
	set_minimum( w, h );
}

////////////////////////////////////////

scroll_area::~scroll_area( void )
{
}

////////////////////////////////////////

void scroll_area::set_delegate( delegate *d )
{
	widget::set_delegate( d );
	_widget->set_delegate( d );
}

////////////////////////////////////////

void scroll_area::paint( const std::shared_ptr<draw::canvas> &canvas )
{
	canvas->save();
	canvas->clip( rectangle() );
	canvas->translate( -_position.x(), -_position.y() );
	_widget->paint( canvas );
	canvas->restore();
}

////////////////////////////////////////

bool scroll_area::mouse_press( const draw::point &p, int button )
{
	return _widget->mouse_press( p + _position, button );
}

////////////////////////////////////////

bool scroll_area::mouse_release( const draw::point &p, int button )
{
	return _widget->mouse_release( p + _position, button );
}

////////////////////////////////////////

bool scroll_area::mouse_move( const draw::point &p )
{
	return _widget->mouse_move( p + _position );
}

////////////////////////////////////////

void scroll_area::compute_layout( void )
{
	_widget->compute_minimum();
	_widget->set_horizontal( 0.0, std::max( _widget->minimum_width(), width() ) - 1.0 );
	_widget->set_vertical( 0.0, std::max( _widget->minimum_height(), height() ) - 1.0 );
	_widget->compute_layout();
}

////////////////////////////////////////

}

