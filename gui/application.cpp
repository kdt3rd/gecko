
#include "application.h"
#include <platform/platform.h>
#include <platform/system.h>

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
	auto const &platforms = platform::platform::list();
	if ( platforms.empty() )
		throw std::runtime_error( "no platforms available" );
	_impl->sys = platforms.front().create();
	_impl->dispatch = _impl->sys->get_dispatcher();
}

////////////////////////////////////////

application::application( const std::string &p )
	: _impl( new application::impl )
{
	auto const &platforms = platform::platform::list();
	if ( platforms.empty() )
		throw std::runtime_error( "no platforms available" );
	if ( p.empty() )
	{
		_impl->sys = platforms.front().create();
		_impl->dispatch = _impl->sys->get_dispatcher();
	}
	else
	{
		for ( size_t i = 0; i < platforms.size(); ++i )
		{
			if ( platforms[i].name() == p )
			{
				_impl->sys = platforms.front().create();
				_impl->dispatch = _impl->sys->get_dispatcher();
				return;
			}
		}
		throw std::runtime_error( "platform does not exist" );
	}
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

