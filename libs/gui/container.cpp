//
// Copyright (c) 2014 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "container.h"
#include <base/scope_guard.h>

namespace gui
{

////////////////////////////////////////

base_container::base_container( void )
{
}

////////////////////////////////////////

base_container::base_container( std::unique_ptr<layout::area> &&a )
	: widget( std::move( a ) )
{
}

////////////////////////////////////////

base_container::~base_container( void )
{
}

////////////////////////////////////////

void base_container::monitor_changed( context &ctxt )
{
	for ( auto &w: _widgets )
		w->monitor_changed( ctxt );
}

////////////////////////////////////////

void base_container::build( context &ctxt )
{
	for ( auto &w: _widgets )
		w->build( ctxt );
}

////////////////////////////////////////

void base_container::paint( context &ctxt )
{
	auto scisguard = ctxt.push_clip( *this );
	gl::api &ogl = ctxt.hw_context().api();

	ogl.clear_color( ctxt.get_style().background_color() );
	ogl.clear();
	for ( auto &w: _widgets )
		w->paint( ctxt );
}

////////////////////////////////////////

std::shared_ptr<widget> base_container::find_widget_under( const point &p )
{
	std::shared_ptr<widget> ret;
	for ( auto &w: _widgets )
	{
		ret = w->find_widget_under( p );
		if ( ret )
			break;
	}
	return ret;
}

////////////////////////////////////////

bool base_container::update_layout( double duration )
{
	bool result = widget::update_layout( duration );
	for ( auto &w: _widgets )
		result = w->update_layout( duration ) | result;
	return result;
}

////////////////////////////////////////

void base_container::remove( size_t w )
{
	_widgets.erase( _widgets.begin() + widget_list::difference_type(w) );
	invalidate();
}

////////////////////////////////////////

template class container_w<layout::grid>;
template class container_w<layout::box>;
template class container_w<layout::tree>;

////////////////////////////////////////

}
