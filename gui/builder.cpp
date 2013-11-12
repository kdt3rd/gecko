
#include "builder.h"
#include "window.h"
#include "application.h"
#include "style.h"
#include <view/cocoa/button.h>
#include <react/button.h>

namespace gui
{

////////////////////////////////////////

builder::builder( const std::shared_ptr<window> &w )
	: _container( w->get_container() )
{
	auto sty = application::current()->get_style();
	auto bg = sty->bg_frame( _container->area() );
	_container->add_view( bg );
}

////////////////////////////////////////

builder::~builder( void )
{
}

////////////////////////////////////////

void builder::make_label( const std::shared_ptr<layout::area> &a, const std::string &txt )
{
	auto sty = application::current()->get_style();
	auto text = sty->label_text( a, txt );
	_container->add_view( text );
}

////////////////////////////////////////

void builder::make_button( const std::shared_ptr<layout::area> &a, const std::string &txt )
{
	auto react = std::make_shared<react::button>( a );
	_container->add_reactor( react );

	auto sty = application::current()->get_style();

	auto frame = sty->button_frame( a );
	_container->add_view( frame );

	auto text = sty->button_text( a, txt );
	_container->add_view( text );

	auto cbutton = std::dynamic_pointer_cast<cocoa::button>( frame );
	if ( !cbutton )
		throw std::runtime_error( "not a cocoa button" );

	react->pressed.callback( [=]( bool p ) { cbutton->set_pressed( p ); } );
}

////////////////////////////////////////

}

