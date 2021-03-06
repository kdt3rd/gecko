// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "window.h"

#include "context.h"

#include <base/contract.h>
#include <base/scope_guard.h>
#include <iostream>
#include <stdexcept>
#include <winuser.h>

namespace platform
{
namespace mswin
{
////////////////////////////////////////

window::window(
    window_type wintype, const std::shared_ptr<screen> &screen, const rect &p )
    : base( wintype, screen, p )
{
    std::cout << "pay attention to window type" << std::endl;
#ifndef MINGW_SDK_INIT
    // TODO: do we need to do this globally???? and MingW does not yet have it
    DPI_AWARENESS_CONTEXT context =
        SetThreadDpiAwarenessContext( DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE );
    on_scope_exit { SetThreadDpiAwarenessContext( context ); };
#endif
    HINSTANCE hInstance = GetModuleHandle( nullptr );
    _hwnd               = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "WindowClass",
        "Title",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        nullptr,
        nullptr,
        hInstance,
        nullptr );

    if ( _hwnd == nullptr )
        throw_runtime( "window creation failed" );

    _context.reset( new context );
    _context->init( _hwnd );
}

////////////////////////////////////////

window::~window( void )
{
    _context.reset();
    if ( _hwnd )
        DestroyWindow( _hwnd );
}

////////////////////////////////////////

::platform::context &window::hw_context( void ) { return ( *_context ); }

////////////////////////////////////////

void window::raise( void )
{
    SetWindowPos( _hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
}

////////////////////////////////////////

void window::lower( void )
{
    SetWindowPos( _hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
}

////////////////////////////////////////

void window::show( void )
{
    ShowWindow( _hwnd, SW_SHOW );
    UpdateWindow( _hwnd );
}

////////////////////////////////////////

void window::hide( void ) { ShowWindow( _hwnd, SW_HIDE ); }

////////////////////////////////////////

bool window::is_visible( void )
{
    return static_cast<bool>( IsWindowVisible( _hwnd ) );
}

////////////////////////////////////////

void window::fullscreen( bool fs )
{
    std::cout << "implement window::fullscreen" << std::endl;
}

////////////////////////////////////////

void window::apply_minimum_size( coord_type w, coord_type h ) {}

////////////////////////////////////////

void window::set_title( const std::string &t ) {}

////////////////////////////////////////

rect window::query_geometry( void )
{
    RECT pos;
    //	GetWindowRect( _hwnd, &pos );
    GetClientRect( _hwnd, &pos );
    return rect{
        pos.left, pos.top, pos.right - pos.left + 1, pos.bottom - pos.top + 1
    };
}

////////////////////////////////////////

bool window::update_geometry( rect &r )
{
    SetWindowPos(
        _hwnd,
        nullptr,
        r.x(),
        r.y(),
        r.width(),
        r.height(),
        SWP_NOOWNERZORDER | SWP_NOZORDER );
    r = query_geometry();
    return true;
}

////////////////////////////////////////

void window::submit_delayed_expose( const rect &r )
{
    /// \todo { Need to fix the local vs screen coordinates }
    RECT rect = { LONG( std::floor( r.x1() ) ),
                  LONG( std::floor( r.y1() ) ),
                  LONG( std::ceil( r.x2() ) ),
                  LONG( std::ceil( r.y2() ) ) };
    if ( rect.left == rect.top && rect.left == rect.right &&
         rect.left == rect.bottom )
        RedrawWindow( _hwnd, nullptr, nullptr, RDW_INTERNALPAINT | RDW_UPDATENOW );
    else
        RedrawWindow( _hwnd, &rect, nullptr, RDW_INVALIDATE | RDW_UPDATENOW );
}

////////////////////////////////////////

void window::make_current( const std::shared_ptr<cursor> &c )
{
    std::cout << "NYI: make_current( cursor )" << std::endl;
}

////////////////////////////////////////

} // namespace mswin
} // namespace platform
