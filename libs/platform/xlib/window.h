// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <GL/glx.h>
#include <X11/Xlib.h>
#include <gl/opengl.h>
#include <platform/window.h>

namespace platform
{
class context;

namespace xlib
{
class system;
class screen;
class context;

////////////////////////////////////////

/// @brief Xlib implementation of platform::window.
class window : public ::platform::window
{
public:
    /// @brief Constrcutor
    window(
        window_type                                wt,
        system &                                   s,
        context *                                  sharectxt,
        const std::shared_ptr<Display> &           dpy,
        const std::shared_ptr<::platform::screen> &scr,
        const rect &                               p = rect( 0, 0, 320, 243 ) );
    ~window( void );

    ::platform::context &hw_context( void ) override;

    void raise( void ) override;
    void lower( void ) override;

    void show( void ) override;
    void hide( void ) override;
    bool is_visible( void ) override;

    void fullscreen( bool fs ) override;

    void set_title( const std::string &t ) override;
    //	void set_icon( const icon &i ) override;

    /// @brief Xlib window identifier.
    Window id( void ) const;
    void   set_input_context( XIC xic ) { _xic = xic; }
    XIC    input_context( void ) const { return _xic; }

protected:
    void apply_minimum_size( coord_type w, coord_type h ) override;

    void make_current( const std::shared_ptr<::platform::cursor> & );

    void submit_delayed_expose( const rect &r ) override;

    rect query_geometry( void ) override;
    bool update_geometry( rect &r ) override;

private:
    void set_win_manager_type( const char *type );

    std::shared_ptr<Display> _display;
    Window                   _win = 0;
    XIC                      _xic = 0;

    coord_type _min_w = 0;
    coord_type _min_h = 0;

    bool _fullscreen = false;

    std::shared_ptr<context> _ctxt;

    GLXContext _glc;
};

////////////////////////////////////////

} // namespace xlib
} // namespace platform
