// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "widget.h"

#include <base/signal.h>
#include <draw/shape.h>

namespace gui
{
////////////////////////////////////////

class checkbox_w : public widget
{
public:
    checkbox_w( void );
    ~checkbox_w( void ) override;

    void build( context &ctxt ) override;
    void paint( context &ctxt ) override;

    bool mouse_press( const event &e ) override;
    bool mouse_release( const event &e ) override;
    bool mouse_move( const event &e ) override;

    bool is_checked( void ) const { return _state; }

    void set_state( bool s );

    base::signal<void( bool )> when_toggled;

public:
    draw::shape _checked;
    draw::shape _unchecked;

    bool _state    = false;
    bool _tracking = false;
    bool _current  = false;
};

////////////////////////////////////////

using checkbox = widget_ptr<checkbox_w>;

} // namespace gui
