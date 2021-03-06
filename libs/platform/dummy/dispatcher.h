// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "keyboard.h"
#include "mouse.h"
#include "window.h"

#include <map>
#include <memory>
#include <platform/dispatcher.h>

namespace platform
{
namespace dummy
{
////////////////////////////////////////

class dispatcher : public platform::dispatcher
{
public:
    dispatcher(
        const std::shared_ptr<keyboard> &k, const std::shared_ptr<mouse> &m );
    ~dispatcher( void );

    int  execute( void ) override;
    void exit( int code ) override;

private:
    int                       _exit_code = 0;
    std::shared_ptr<keyboard> _keyboard;
    std::shared_ptr<mouse>    _mouse;
};

////////////////////////////////////////

} // namespace dummy
} // namespace platform
