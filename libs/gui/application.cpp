//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "application.h"
#include "window.h"
#include <platform/platform.h>
#include <platform/system.h>
#include <script/font_manager.h>
#include <base/contract.h>

namespace
{
	static std::vector<std::weak_ptr<gui::application>> stack;
}

namespace gui
{

struct application::impl
{
	std::shared_ptr<platform::system> sys;
	std::shared_ptr<platform::dispatcher> dispatch;
};

////////////////////////////////////////

application::application( const std::string &p, const std::string &r )
	: _impl( new application::impl )
{
	auto const &platforms = platform::platform::list();
	if ( platforms.empty() )
		throw std::runtime_error( "no platforms available" );

	for ( const auto &plat: platforms )
	{
		if ( p.empty() || ( plat.name() == p ) )
		{
			if ( r.empty() || ( plat.renderer() == r ) )
			{
				auto sys = plat.create();
				if ( sys->is_working() )
				{
					_platform = plat.name() + "+" + plat.renderer();
					_impl->sys = plat.create();
					_impl->dispatch = _impl->sys->get_dispatcher();
					break;
				}
			}
		}
	}

	if ( !_impl->sys )
		throw std::runtime_error( "platform does not exist" );

	_fmgr = script::font_manager::common();
	if ( !_fmgr )
		throw std::runtime_error( "no font manager available" );
}

////////////////////////////////////////

application::~application( void )
{
}

////////////////////////////////////////

std::shared_ptr<window> application::new_window( void )
{
	auto w = _impl->sys->new_window();
	return std::make_shared<window>( w );
}

////////////////////////////////////////

std::shared_ptr<popup> application::new_popup( void )
{
	throw_not_yet();
//	auto w = _impl->sys->new_window();
//	return std::make_shared<popup>( w );
}

////////////////////////////////////////

std::shared_ptr<menu> application::new_menu( void )
{
	throw_not_yet();
//	auto w = _impl->sys->new_window();
//	return std::make_shared<menu>( w );
}

////////////////////////////////////////

int application::run( void )
{
	return _impl->dispatch->execute();
}

////////////////////////////////////////

void application::exit( int code )
{
	_impl->dispatch->exit( code );
}

////////////////////////////////////////

void application::push( void )
{
#if __cplusplus >= 201703
	stack.emplace_back( weak_from_this() );
#else
	std::shared_ptr<application> v = shared_from_this();
	stack.emplace_back( v );
#endif
}

////////////////////////////////////////

void application::pop( void )
{
	precondition( !stack.empty(), "popping empty stack" );
	auto ptr = stack.back().lock();
	precondition( ptr.get() == this, "popping wrong application" );
	stack.pop_back();
}

////////////////////////////////////////

std::set<std::string> application::get_font_families( void )
{
	return _fmgr->get_families();
}

////////////////////////////////////////

std::set<std::string> application::get_font_styles( const std::string &family )
{
	return _fmgr->get_styles( family );
}

////////////////////////////////////////

std::shared_ptr<script::font> application::get_font( const std::string &family, const std::string &style, double pixsize )
{
	return _fmgr->get_font( family, style, pixsize );
}

////////////////////////////////////////

std::shared_ptr<script::font> application::get_default_font( void )
{
	return _fmgr->get_font( "Lucida Grande", "Regular", 14.0 );
}

////////////////////////////////////////

std::shared_ptr<application> application::current( void )
{
	precondition( !stack.empty(), "getting empty stack" );
	auto ptr = stack.back().lock();
	precondition( bool(ptr), "deleted application" );
	return ptr;
}

////////////////////////////////////////

}

