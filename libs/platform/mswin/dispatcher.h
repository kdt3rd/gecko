// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "keyboard.h"
#include "mouse.h"
#include "window.h"

#include <atomic>
#include <deque>
#include <map>
#include <memory>
#include <platform/dispatcher.h>
#include <platform/event.h>

namespace platform
{
namespace mswin
{
////////////////////////////////////////

class dispatcher : public ::platform::dispatcher
{
public:
    dispatcher( ::platform::system *sys );
    ~dispatcher( void ) override;

    int  execute( void ) override;
    void exit( int code ) override;

    void add_window( const std::shared_ptr<window> &win );
    void remove_window( const std::shared_ptr<window> &win );

    bool deliver_event( HWND hw, UINT msg, WPARAM wp, LPARAM lp );
    void add_event( HWND hw, UINT msg, WPARAM wp, LPARAM lp );

private:
    struct evt
    {
        evt() = default;
        evt( HWND h, UINT m, WPARAM w, LPARAM l ) noexcept
            : hwnd( h ), msg( m ), wp( w ), lp( l )
        {}

        HWND   hwnd = 0;
        UINT   msg  = 0;
        WPARAM wp   = 0;
        LPARAM lp   = 0;
    };
    bool dispatch_evt( const evt &e );

    void send_mouse_evt(
        const std::shared_ptr<window> &w,
        event_type                     e,
        int                            button,
        WPARAM                         wp,
        LPARAM                         lp );
    uint8_t extract_mods( WORD v );

    ::platform::system *_system = nullptr;

    std::deque<evt>                         _events;
    std::map<HWND, std::shared_ptr<window>> _windows;

    int               _exit_code = 0;
    std::atomic<bool> _exit_requested{ false };

    std::shared_ptr<keyboard> _keyboard;
    std::shared_ptr<mouse>    _mouse;

    ATOM _normal_window_class = 0;
};

////////////////////////////////////////

} // namespace mswin
} // namespace platform
