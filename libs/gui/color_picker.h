// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "widget.h"

namespace gui
{
////////////////////////////////////////

class color_picker : public widget
{
public:
    color_picker( void );

    void paint( const std::shared_ptr<draw::canvas> &c ) override;

    bool mouse_press( const point &p, int b ) override;
    bool mouse_release( const point &p, int b ) override;
    bool mouse_move( const point &p ) override;

private:
    bool  _tracking = false;
    color _current;
};

////////////////////////////////////////

} // namespace gui
