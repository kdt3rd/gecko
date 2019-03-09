// Copyright (c) 2017 Ian Godin
// SPDX-License-Identifier: MIT

#include "composite.h"
#include "label.h"
#include "radio_button.h"

namespace gui
{

////////////////////////////////////////

composite::~composite( void )
{
}

////////////////////////////////////////

void composite::monitor_changed( context &ctxt )
{
	for_subwidgets( [&]( const std::shared_ptr<widget> &w )
	{
		w->monitor_changed( ctxt );
	} );
}

////////////////////////////////////////

void composite::build( context &ctxt )
{
	for_subwidgets( [&]( const std::shared_ptr<widget> &w )
	{
		w->build( ctxt );
	} );
}

////////////////////////////////////////

void composite::paint( context &ctxt )
{
	for_subwidgets( [&]( const std::shared_ptr<widget> &w )
	{
		w->paint( ctxt );
	} );
}

////////////////////////////////////////

std::shared_ptr<widget> composite::find_widget_under( const point &p )
{
	std::shared_ptr<widget> ret;
	for_subwidgets( [&]( const std::shared_ptr<widget> &w )
	{
		if ( ret )
			return;
		ret = w->find_widget_under( p );
	} );
	return ret;
}

////////////////////////////////////////

bool composite::update_layout( double duration )
{
	bool result = widget::update_layout( duration );
	for_subwidgets( [&]( const std::shared_ptr<widget> &w )
	{
		result = w->update_layout( duration ) | result;
	} );
	return result;
}

////////////////////////////////////////

}
