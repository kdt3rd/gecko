// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "background.h"

namespace gui
{
////////////////////////////////////////

class background_color : public background
{
public:
    background_color(
        const color &                  c = { 0.13, 0.13, 0.13, 1 },
        const std::shared_ptr<widget> &w = std::shared_ptr<widget>() );
    ~background_color( void );

    void set_color( const color &c ) { _color = c; }

    void paint( const std::shared_ptr<draw::canvas> &c ) override;

private:
    color _color = { 0.13, 0.13, 0.13, 1 };
};

////////////////////////////////////////

} // namespace gui
