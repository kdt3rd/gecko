// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "scroll_bar.h"
#include "widget.h"

#include <layout/scroll.h>

namespace gui
{
////////////////////////////////////////

class scroll_area_w : public widget
{
public:
    using value_type = scroll_bar_w::value_type;

    scroll_area_w(
        bool hscroll = true, bool vscroll = true, bool bounded = true );
    ~scroll_area_w( void ) override;

    void set_widget( const std::shared_ptr<widget> &v );
    template <typename Y> inline void set_widget( const widget_ptr<Y> &w )
    {
        set_widget( static_cast<std::shared_ptr<Y>>( w ) );
    }

    void build( context &ctxt ) override;
    void paint( context &ctxt ) override;

    std::shared_ptr<widget> find_widget_under( const point &p ) override;

    bool update_layout( double duration ) override;

private:
    void update_widget( void );

    bool  _tracking = false;
    point _track;

    std::shared_ptr<layout::scroll> _layout;
    std::shared_ptr<widget>         _main;
    std::shared_ptr<widget>         _widget;
    std::shared_ptr<scroll_bar_w>   _hscroll;
    std::shared_ptr<scroll_bar_w>   _vscroll;
};

////////////////////////////////////////

using scroll_area = widget_ptr<scroll_area_w>;

} // namespace gui
