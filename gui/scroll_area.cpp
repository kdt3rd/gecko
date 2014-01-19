
#include "scroll_area.h"

namespace gui
{

////////////////////////////////////////

scroll_area::scroll_area( scroll_behavior hscroll, scroll_behavior vscroll )
	: _hscroll( hscroll ), _vscroll( vscroll )
{
}

////////////////////////////////////////

scroll_area::~scroll_area( void )
{
}

////////////////////////////////////////

void scroll_area::paint( const std::shared_ptr<draw::canvas> &canvas )
{
	if ( _widget )
	{
		canvas->save();
// TODO		canvas->clip( *this );
		_widget->paint( canvas );
		canvas->restore();
	}
}

////////////////////////////////////////

bool scroll_area::mouse_press( const core::point &p, int button )
{
	if ( button == 2 && _widget )
	{
		_tracking = true;
		_track = p;
		return true;
	}
	else
	{
		if ( _widget )
			return _widget->mouse_press( p, button );
		return widget::mouse_press( p, button );
	}
}

////////////////////////////////////////

bool scroll_area::mouse_release( const core::point &p, int button )
{
	if ( _tracking )
	{
		_tracking = false;
		return true;
	}
	else
	{
		if ( _widget )
			return _widget->mouse_release( p, button );
		return widget::mouse_release( p, button );
	}
}

////////////////////////////////////////

bool scroll_area::mouse_move( const core::point &p )
{
	if ( _tracking )
	{
		if ( _widget )
			_widget->set_position( _widget->position() - _track.delta( p ) );
		compute_layout();
		_track = p;
		invalidate();
		return true;
	}
	else
	{
		if ( _widget )
			return _widget->mouse_move( p );
		return widget::mouse_move( p );
	}
}

////////////////////////////////////////

void scroll_area::compute_minimum( void )
{
	if ( _widget )
	{
		_widget->compute_minimum();
		switch( _hscroll )
		{
			case scroll_behavior::FREE:
			case scroll_behavior::BOUND:
				break;

			case scroll_behavior::NONE:
				set_minimum_width( _widget->minimum_width() );
				break;
		}
		switch( _vscroll )
		{
			case scroll_behavior::FREE:
			case scroll_behavior::BOUND:
				break;

			case scroll_behavior::NONE:
				set_minimum_height( _widget->minimum_height() );
				break;
		}
	}
}

////////////////////////////////////////

void scroll_area::compute_layout( void )
{
	if ( _widget )
	{
		switch( _hscroll )
		{
			case scroll_behavior::FREE:
				_widget->set_width( _widget->minimum_width() );
				break;

			case scroll_behavior::BOUND:
				_widget->set_width( _widget->minimum_width() );
				if ( _widget->x() > x() )
					_widget->set_x( x() );
				if ( _widget->x2() < x2() )
				{
					if ( _widget->minimum_width() < width() )
					{
						_widget->set_x1( x1() );
						_widget->set_x2( x2() );
					}
					else
						_widget->move_by( _widget->x2() - x2(), 0 );
				}
				break;

			case scroll_behavior::NONE:
				_widget->set_x( x() );
				_widget->set_width( width() );
				break;
		}
		switch( _vscroll )
		{
			case scroll_behavior::FREE:
				_widget->set_height( _widget->minimum_height() );
				break;

			case scroll_behavior::BOUND:
				_widget->set_height( _widget->minimum_height() );
				if ( _widget->y() > y() )
					_widget->set_y( y() );
				if ( _widget->y2() < y2() )
				{
					if ( _widget->minimum_height() < height() )
					{
						_widget->set_y1( y1() );
						_widget->set_y2( y2() );
					}
					else
						_widget->move_by( 0, y2() - _widget->y2() );
				}
				break;

			case scroll_behavior::NONE:
				_widget->set_y( y() );
				_widget->set_height( height() );
				break;
		}
		_widget->compute_layout();
	}
}

////////////////////////////////////////

}

