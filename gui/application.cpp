
#define PLATFORM_H <platform/PLATFORM/system.h>
#include PLATFORM_H

#include "application.h"

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

application::application( void )
	: _impl( new application::impl )
{
	_impl->sys = std::make_shared<platform::native_system>();
	_impl->dispatch = _impl->sys->get_dispatcher();
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

std::shared_ptr<draw::font> application::get_font( const std::string &family, const std::string &style, double pixsize )
{
	auto fmgr = _impl->sys->get_font_manager();
	return fmgr->get_font( family, style, pixsize );
}

////////////////////////////////////////

std::shared_ptr<application> application::current( void )
{
	precondition( !stack.empty(), "getting empty stack" );
	auto ptr = stack.back().lock();
	precondition( bool(ptr), "null application" );
	return ptr;
}

////////////////////////////////////////

}

