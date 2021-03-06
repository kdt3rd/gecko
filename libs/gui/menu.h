// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "container.h"
#include "window.h"

namespace gui
{
////////////////////////////////////////

class menu_w : public widget
{
public:
    menu_w( const window &w );
    ~menu_w( void ) override;

    void add_entry( const std::string &utf8label, const action &a );
    void add_separator( void );

private:
    std::shared_ptr<gui::simple_container> _container;
};

////////////////////////////////////////

using menu = widget_ptr<menu_w>;
} // namespace gui
