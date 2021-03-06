// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "application.h"
#include "widget.h"

#include <draw/rectangle.h>
#include <draw/text.h>

namespace gui
{
////////////////////////////////////////

class line_edit_w : public widget
{
public:
    line_edit_w( void );
    line_edit_w( std::string l );
    ~line_edit_w( void ) override;

    const std::string &text( void ) const { return _text.get_text(); }
    void               set_text( const std::string &t ) { _text.set_text( t ); }
    void set_prompt( const std::string &p ) { _prompt.set_text( p ); }

    void set_font( std::shared_ptr<script::font> &f )
    {
        _text.set_font( f );
        _prompt.set_font( f );
    }

    void build( context &ctxt ) override;
    void paint( context &ctxt ) override;

    bool key_press( const event &e ) override;

    bool text_input( const event &e ) override;

    bool mouse_press( const event &e ) override;

private:
    draw::rectangle _line;
    draw::rectangle _marker;
    draw::text      _text;
    draw::text      _prompt;
    size_t          _cursor = 0;
};

////////////////////////////////////////

using line_edit = widget_ptr<line_edit_w>;

} // namespace gui
