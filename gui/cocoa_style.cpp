
#include "cocoa_style.h"
#include <core/contract.h>
#include <view/text.h>
#include <view/flat.h>
#include <view/cocoa/button.h>
#include "application.h"

namespace gui
{

////////////////////////////////////////

cocoa_style::cocoa_style( void )
{
	_default_font = application::current()->get_font( "Lucida Grande", "Regular", 14.0 );
	postcondition( bool(_default_font), "font for cocoa style not found" );
}

////////////////////////////////////////

cocoa_style::~cocoa_style( void )
{
}

////////////////////////////////////////

std::shared_ptr<draw::font> cocoa_style::default_font( void )
{
	return _default_font;
}

////////////////////////////////////////

std::shared_ptr<view::view> cocoa_style::bg_frame( const std::shared_ptr<layout::area> &area )
{
	auto ret = std::make_shared<view::flat>( area );
	ret->set_color( { 0.9294, 0.9294, 0.9294, 1.0 } );
	return ret;
}

////////////////////////////////////////

std::shared_ptr<view::view> cocoa_style::button_frame( const std::shared_ptr<layout::area> &area )
{
	return std::make_shared<cocoa::button>( area );
}

////////////////////////////////////////

std::shared_ptr<view::view> cocoa_style::button_text( const std::shared_ptr<layout::area> &area, const std::string &txt )
{
	auto ret = std::make_shared<view::text>( area );
	ret->set_font( _default_font );
	ret->set_color( { 0.1961, 0.1961, 0.1961, 1.0 } );
	ret->set_text( txt );
	return ret;
}

////////////////////////////////////////

std::shared_ptr<view::view> cocoa_style::label_text( const std::shared_ptr<layout::area> &area, const std::string &txt )
{
	auto ret = std::make_shared<view::text>( area );
	ret->set_font( _default_font );
	ret->set_color( { 0.1961, 0.1961, 0.1961, 1.0 } );
	ret->set_text( txt );
	return ret;
}

////////////////////////////////////////

}

