//
// Copyright (c) 2014-2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "scroll_area.h"

namespace gui
{

////////////////////////////////////////

scroll_area::scroll_area( bool hscroll, bool vscroll, bool bounded )
	: widget( std::unique_ptr<layout::scroll_layout>( new layout::scroll_layout ) )
{
	_layout = std::dynamic_pointer_cast<layout::scroll_layout>( layout_target() );

	if ( hscroll )
	{
		_hscroll = std::make_shared<scroll_bar>( bounded );
		_hscroll->set_horizontal();
		_layout->set_hscroll( _hscroll->layout_target() );
	}

	if ( vscroll )
	{
		_vscroll = std::make_shared<scroll_bar>( bounded );
		_vscroll->set_vertical();
		_layout->set_vscroll( _vscroll->layout_target() );
	}

	_main = std::make_shared<layout::area>();
	_layout->set_main( _main );
}

////////////////////////////////////////

scroll_area::~scroll_area( void )
{
}

////////////////////////////////////////

void scroll_area::set_widget( const std::shared_ptr<widget> &v )
{
	_widget = v;
	_layout->set_sublayout( _widget->layout_target() );
	update_widget();
}

////////////////////////////////////////

void scroll_area::build( gl::api &ogl )
{
	if ( _hscroll )
		_hscroll->build( ogl );
	if ( _vscroll )
		_vscroll->build( ogl );
//	if ( _corner )
//		_corner->build( ogl );
	if ( _widget )
		_widget->build( ogl );
}

////////////////////////////////////////

void scroll_area::paint( gl::api &ogl )
{
	if ( _hscroll )
		_hscroll->paint( ogl );
	if ( _vscroll )
		_vscroll->paint( ogl );
//	if ( _corner )
//		_corner->paint( ogl );

	if ( _widget )
	{
		ogl.push_scissor( _main->x(), _main->y(), _main->width(), _main->height() );
		{
			ogl.save_matrix();
			{
				double dx = 0.0, dy = 0.0;
				if ( _hscroll )
					dx = _hscroll->value();
				if ( _vscroll )
					dy = _vscroll->value();
				ogl.model_matrix().translate( _main->x() - dx, _main->y() - dy );

				_widget->paint( ogl );
			}
			ogl.restore_matrix();
		}
		ogl.pop_scissor();
	}
}

////////////////////////////////////////

bool scroll_area::mouse_press( const base::point &p, int button )
{
	bool result = false;
	if ( !result && _hscroll )
		result = _hscroll->mouse_press( p, button );
	if ( !result && _vscroll )
		result = _vscroll->mouse_press( p, button );
	return result;

//	if ( button == 2 && _widget )
//	{
//		_tracking = true;
//		_track = p;
//		return true;
//	}
//	else
//	{
//		if ( _widget )
//			return _widget->mouse_press( p, button );
//		return widget::mouse_press( p, button );
//	}
}

////////////////////////////////////////

bool scroll_area::mouse_release( const base::point &p, int button )
{
	bool result = false;
	if ( !result && _hscroll )
		result = _hscroll->mouse_release( p, button );
	if ( !result && _vscroll )
		result = _vscroll->mouse_release( p, button );
	return result;

//	if ( _tracking )
//	{
//		_tracking = false;
//		return true;
//	}
//	else
//	{
//		if ( _widget )
//			return _widget->mouse_release( p, button );
//		return widget::mouse_release( p, button );
//	}
}

////////////////////////////////////////

bool scroll_area::mouse_move( const base::point &p )
{
	bool result = false;
	if ( !result && _hscroll )
		result = _hscroll->mouse_move( p );
	if ( !result && _vscroll )
		result = _vscroll->mouse_move( p );
	return result;

//	if ( _tracking )
//	{
//		if ( _widget )
//			_widget->set_position( _widget->position() - _track.delta( p ) );
//		_track = p;
//		invalidate();
//		return true;
//	}
//	else
//	{
//		if ( _widget )
//			return _widget->mouse_move( p );
//		return widget::mouse_move( p );
//	}
}

////////////////////////////////////////

bool scroll_area::update_layout( double duration )
{
	bool result = widget::update_layout( duration );

	if ( _hscroll )
		result = _hscroll->update_layout( duration ) | result;
	if ( _vscroll )
		result = _vscroll->update_layout( duration ) | result;

	if ( _widget )
	{
		result = _widget->update_layout( duration ) | result;
		update_widget();
	}

	return result;
}

////////////////////////////////////////

void scroll_area::update_widget( void )
{
	if ( _widget )
	{
		if ( _widget->width() > 0.0 )
		{
			_hscroll->set_range( _widget->x1(), _widget->x2() + 1 );
			_hscroll->set_handle( _main->width() );
		}
		if ( _widget->height() > 0.0 )
		{
			_vscroll->set_range( _widget->y1(), _widget->y2() + 1 );
			_vscroll->set_handle( _main->height() );
		}
	}
	else
	{
		_hscroll->set_range( 0, 100 );
		_vscroll->set_range( 0, 100 );
	}
}

////////////////////////////////////////

}

