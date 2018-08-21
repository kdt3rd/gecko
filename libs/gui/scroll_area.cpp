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

scroll_area_w::scroll_area_w( bool hscroll, bool vscroll, bool bounded )
	: widget( std::unique_ptr<layout::scroll>( new layout::scroll ) )
{
	_layout = std::dynamic_pointer_cast<layout::scroll>( layout_target() );

	if ( hscroll )
	{
		_hscroll = std::make_shared<scroll_bar_w>( bounded );
		_hscroll->set_horizontal();
		_layout->set_hscroll( _hscroll->layout_target() );
	}

	if ( vscroll )
	{
		_vscroll = std::make_shared<scroll_bar_w>( bounded );
		_vscroll->set_vertical();
		_layout->set_vscroll( _vscroll->layout_target() );
	}

	_main = std::make_shared<widget>();
	_layout->set_main( _main->layout_target() );
}

////////////////////////////////////////

scroll_area_w::~scroll_area_w( void )
{
}

////////////////////////////////////////

void scroll_area_w::set_widget( const std::shared_ptr<widget> &v )
{
	_widget = v;
	_layout->set_sublayout( _widget->layout_target() );
	update_widget();
}

////////////////////////////////////////

void scroll_area_w::build( context &ctxt )
{
	if ( _hscroll )
		_hscroll->build( ctxt );
	if ( _vscroll )
		_vscroll->build( ctxt );
//	if ( _corner )
//		_corner->build( ctxt );
	if ( _widget )
		_widget->build( ctxt );
}

////////////////////////////////////////

void scroll_area_w::paint( context &ctxt )
{
	if ( _hscroll )
		_hscroll->paint( ctxt );
	if ( _vscroll )
		_vscroll->paint( ctxt );
//	if ( _corner )
//		_corner->paint( ctxt );

	if ( _widget )
	{
		auto scisguard = ctxt.push_clip( *_main );
		platform::context &hwc = ctxt.hw_context();
		{
			gl::api &ogl = hwc.api();
			ogl.save_matrix();
			{
				value_type dx = value_type( 0 ), dy = value_type( 0 );
				if ( _hscroll )
					dx = _hscroll->value();
				if ( _vscroll )
					dy = _vscroll->value();
				ogl.model_matrix().translate( draw::to_api( _main->x() - dx ),
				                              draw::to_api( _main->y() - dy ) );

				_widget->paint( ctxt );
			}
			ogl.restore_matrix();
		}
	}
}

////////////////////////////////////////

std::shared_ptr<widget> scroll_area_w::find_widget_under( const point &p )
{
	if ( _hscroll && _hscroll->contains( p ) )
		return _hscroll;
	if ( _vscroll && _vscroll->contains( p ) )
		return _vscroll;

	// if we're not in the scroll areas, we're in the widget
	if ( _widget )
		return _widget;

	return widget::find_widget_under( p );
}

////////////////////////////////////////

bool scroll_area_w::update_layout( double duration )
{
	bool result = widget::update_layout( duration );

	if ( _hscroll )
		result = _hscroll->update_layout( duration ) || result;
	if ( _vscroll )
		result = _vscroll->update_layout( duration ) || result;
	result = _main->update_layout( duration ) || result;

	if ( _widget )
	{
		result = _widget->update_layout( duration ) || result;
		update_widget();
	}

	return result;
}

////////////////////////////////////////

void scroll_area_w::update_widget( void )
{
	if ( _widget )
	{
		if ( _widget->width() > coord( 0 ) )
		{
			_hscroll->set_range( _widget->x1().count(), _widget->x1().count() + _widget->width().count() );
			_hscroll->set_handle( _main->width().count() );
		}
		if ( _widget->height() > coord( 0 ) )
		{
			_vscroll->set_range( _widget->y1().count(), _widget->y1().count() + _widget->height().count() );
			_vscroll->set_handle( _main->height().count() );
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
