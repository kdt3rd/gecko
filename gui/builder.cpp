
#include "builder.h"
#include "window.h"
#include "application.h"
#include "style.h"
#include <view/cocoa/button.h>

namespace gui
{

////////////////////////////////////////

builder::builder( const std::shared_ptr<window> &w )
	: _container( w->get_container() )
{
}

////////////////////////////////////////

builder::~builder( void )
{
}

////////////////////////////////////////

void builder::make_label( const std::shared_ptr<draw::area> &a, const std::string &txt )
{
	auto sty = application::current()->get_style();
	auto text = sty->label_text( a, txt );
	_container->add_view( text );
}

////////////////////////////////////////

void builder::make_button( const std::shared_ptr<draw::area> &a, const std::string &txt )
{
	auto sty = application::current()->get_style();

	auto frame = sty->button_frame( a );
	_container->add_view( frame );

	auto text = sty->button_text( a, txt );
	_container->add_view( text );
}

////////////////////////////////////////

}

