// Copyright (c) 2014 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include "application.h"

#include "event.h"
#include "window.h"

#include <base/contract.h>
#include <platform/dispatcher.h>
#include <platform/event.h>
#include <platform/platform.h>
#include <platform/system.h>
#include <platform/window.h>
#include <script/font_manager.h>

namespace
{
static std::vector<std::weak_ptr<gui::application>> stack;
}

namespace gui
{
struct application::impl
{
    std::shared_ptr<platform::system>     sys;
    std::shared_ptr<platform::dispatcher> dispatch;
};

////////////////////////////////////////

application::application(
    const std::string &display, const std::string &p, const std::string &r )
    : _impl( new application::impl ), _fmgr( script::font_manager::make() )
{
    auto const &platforms = platform::platform::list();
    if ( platforms.empty() )
        throw_runtime( "no platforms available" );

    for ( const auto &plat: platforms )
    {
        if ( p.empty() || ( plat.name() == p ) )
        {
            if ( r.empty() || ( plat.renderer() == r ) )
            {
                auto sys = plat.create( display );
                if ( sys->is_working() )
                {
                    _platform       = plat.name() + "+" + plat.renderer();
                    _impl->sys      = sys;
                    _impl->dispatch = _impl->sys->get_dispatcher();
                    break;
                }
            }
        }
    }

    if ( !_impl->sys )
        throw std::runtime_error( "platform does not exist" );
}

////////////////////////////////////////

application::~application( void )
{
    _impl->dispatch.reset();
    _impl->sys.reset();
}

////////////////////////////////////////

bool application::process_quit_request( void ) { return true; }

////////////////////////////////////////

void application::register_global_hotkey(
    platform::scancode sc, hotkey_handler f )
{
    // TODO: handle duplicates?
    _hotkeys[sc] = std::move( f );
}

////////////////////////////////////////

bool application::dispatch_global_hotkey( const event &e )
{
    platform::scancode sc = e.raw_key().keys[0];

    auto i = _hotkeys.find( sc );
    if ( i != _hotkeys.end() )
    {
        ( i->second )( e.from_native( e.raw_key().x, e.raw_key().y ) );
        return true;
    }

    return false;
}

////////////////////////////////////////

std::shared_ptr<window>
application::new_window( const std::shared_ptr<platform::screen> &s )
{
    auto result = std::make_shared<window>(
        _impl->sys->new_window( platform::window_type::normal, s ) );

    update_display( result.get() );

    return result;
}

////////////////////////////////////////

void application::update_display( window *w )
{
    if ( !w )
        return;

    auto nw = w->native_window();
    if ( nw )
    {
        auto scr = nw->query_screen();
        if ( scr )
        {
            auto  dpi = scr->dpi();
            GLint mw  = 1024;

            {
                auto guard = w->bind();
                glGetIntegerv( GL_MAX_TEXTURE_SIZE, &mw );
            }

            w->get_style().set_font_cache(
                _fmgr->get_cache( dpi.w(), dpi.h(), mw, mw ) );
        }
    }
}

////////////////////////////////////////

void application::window_destroyed( window * /*w*/ )
{
    // TBD: we aren't stashing the windows (yet), so until then, just ignore...
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

std::shared_ptr<cursor> application::new_cursor( void )
{
    return _impl->sys->new_cursor();
}

////////////////////////////////////////

std::shared_ptr<cursor> application::builtin_cursor( standard_cursor sc )
{
    return _impl->sys->builtin_cursor( sc );
}

////////////////////////////////////////

int application::run( void ) { return _impl->dispatch->execute(); }

////////////////////////////////////////

void application::exit( int code ) { _impl->dispatch->exit( code ); }

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

std::shared_ptr<platform::system> application::get_system( void )
{
    return _impl->sys;
}

////////////////////////////////////////

std::shared_ptr<application> application::current( void )
{
    precondition( !stack.empty(), "getting empty stack" );
    auto ptr = stack.back().lock();
    precondition( bool( ptr ), "deleted application" );
    return ptr;
}

////////////////////////////////////////

} // namespace gui
