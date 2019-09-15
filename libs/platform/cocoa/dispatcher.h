// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "keyboard.h"
#include "mouse.h"
#include "platform.h"
#include "window.h"

#include <map>
#include <memory>
#include <platform/dispatcher.h>
#include <vector>

namespace platform
{
namespace cocoa
{
////////////////////////////////////////

/// @brief Cocoa implementation of dispatcher.
class dispatcher : public ::platform::dispatcher
{
public:
    dispatcher( ::platform::system *s );
    ~dispatcher( void ) override;

    int  execute( void ) override;
    void exit( int code ) override;

    void add_waitable( const std::shared_ptr<waitable> &w ) override;
    void remove_waitable( const std::shared_ptr<waitable> &w ) override;

    void add_window( const std::shared_ptr<window> &w );
    void remove_window( const std::shared_ptr<window> &w );

private:
    int _exit_code = 0;
    //	bool _continue_running = true;
    std::shared_ptr<keyboard>                 _keyboard;
    std::shared_ptr<mouse>                    _mouse;
    std::map<void *, std::shared_ptr<window>> _windows;

    CGEventSourceRef _event_source;
};

////////////////////////////////////////

} // namespace cocoa
} // namespace platform
