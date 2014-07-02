
#include "application.h"
#include <platform/platform.h>
#include <platform/system.h>
#include <script/fontconfig/font_manager.h>

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

/*
application::application( void )
	: _impl( new application::impl )
{
	auto const &platforms = platform::platform::list();
	if ( platforms.empty() )
		throw std::runtime_error( "no platforms available" );

	const platform::platform &p = platforms.front();
	_platform = p.name() + "+" + p.render();
	_impl->sys = p.create();
	_impl->dispatch = _impl->sys->get_dispatcher();
}
*/

////////////////////////////////////////

application::application( const std::string &p, const std::string &r )
	: _impl( new application::impl )
{
	auto const &platforms = platform::platform::list();
	if ( platforms.empty() )
		throw std::runtime_error( "no platforms available" );

	for ( size_t i = 0; i < platforms.size(); ++i )
	{
		if ( p.empty() || ( platforms[i].name() == p ) )
		{
			if ( r.empty() || ( platforms[i].render() == r ) )
			{
				std::cout << p << ' ' << platforms[i].name() << std::endl;
				const platform::platform &p = platforms[i];
				_platform = p.name() + "+" + p.render();
				_impl->sys = p.create();
				_impl->dispatch = _impl->sys->get_dispatcher();
				break;
			}
		}
	}

	if ( !_impl->sys )
		throw std::runtime_error( "platform does not exist" );

	_fmgr = std::make_shared<script::fontconfig::font_manager>();
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
	auto w = _impl->sys->new_window();
	return std::make_shared<popup>( w );
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
	stack.push_back( shared_from_this() );
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

std::shared_ptr<application> application::current( void )
{
	precondition( !stack.empty(), "getting empty stack" );
	auto ptr = stack.back().lock();
	precondition( bool(ptr), "deleted application" );
	return ptr;
}

////////////////////////////////////////

const std::shared_ptr<style> &application::current_style( void )
{
	return current()->get_style();
}

////////////////////////////////////////

}

