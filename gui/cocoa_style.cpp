
#include "cocoa_style.h"
#include <core/contract.h>
#include <view/text.h>
#include <view/cocoa/button.h>
#include "application.h"

namespace gui
{

////////////////////////////////////////

cocoa_style::cocoa_style( void )
{
	_font_button = application::current()->get_font( "Lucida Grande", "Bold", 14.0 );
	postcondition( bool(_font_button), "font for Cocoa style not found" );
}

////////////////////////////////////////

cocoa_style::~cocoa_style( void )
{
}

////////////////////////////////////////

std::shared_ptr<view::view> cocoa_style::button_frame( const std::shared_ptr<draw::area> &area )
{
	return std::make_shared<cocoa::button>( area );
}

////////////////////////////////////////

std::shared_ptr<view::view> cocoa_style::button_text( const std::shared_ptr<draw::area> &area, const std::string &txt )
{
	auto ret = std::make_shared<view::text>( area );
	ret->set_font( _font_button );
	ret->set_color( { 0.1961, 0.1961, 0.1961, 1.0 } );
	ret->set_text( txt );
	return ret;
}

////////////////////////////////////////

std::shared_ptr<view::view> cocoa_style::label_text( const std::shared_ptr<draw::area> &area, const std::string &txt )
{
	auto ret = std::make_shared<view::text>( area );
	ret->set_font( _font_button );
	ret->set_color( { 0.1961, 0.1961, 0.1961, 1.0 } );
	ret->set_text( txt );
	return ret;
}

////////////////////////////////////////

}

