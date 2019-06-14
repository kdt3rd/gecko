// Copyright (c) 2018 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include "window.h"

////////////////////////////////////////

namespace platform
{
///
/// @brief Class simple_window provides...
///
class simple_window
{
public:
    explicit simple_window( const std::shared_ptr<window> &win );

    const std::shared_ptr<screen> &query_screen( void ) const
    {
        return _win->query_screen();
    }

    /// @brief creates a context (or returns a stashed version)
    ///
    /// This is returned as a shared pointer, for caching
    /// (performance) purposes, however, it is not intended to have a
    /// lifetime longer than a window, and if used past the
    /// destruction of a window, errors should be expected.
    context &hw_context( void ) { return _win->hw_context(); }

    /// @brief Set a cursor as the default for the window
    ///
    /// This is the cursor that is displayed if no other cursor is pushed
    ///
    void set_default_cursor( const std::shared_ptr<cursor> &c )
    {
        _win->set_default_cursor( c );
    }
    /// @brief Set a new cursor for the window, storing them as a stack
    void push_cursor( const std::shared_ptr<cursor> &c )
    {
        _win->push_cursor( c );
    }
    /// @brief restore the previous cursor
    void pop_cursor( void ) { _win->pop_cursor(); }

    // TODO: add support for menus...
    //	std::shared_ptr<menu_definition> top_level_menu( void );
    //	virtual bool include_menu_in_window_area( void ) const = 0;

    /// @brief Raise the window.
    ///
    /// Raise the window above all other windows.
    void raise( void ) { _win->raise(); }

    /// @brief Lower the window.
    ///
    /// Lower the window below all other windows.
    void lower( void ) { _win->lower(); }

    /// @brief Show the window.
    ///
    /// Make the window visible.
    void show( void ) { _win->show(); }

    /// @brief Hide the window.
    ///
    /// Make the window invisible.
    void hide( void ) { _win->hide(); }

    /// @brief Query if the window is visible.
    ///
    /// @return Whether the window is visible or not
    bool is_visible( void ) { return _win->is_visible(); }

    /// @brief Make the window fullscreen.
    ///
    /// Make the window fullscreen.
    void fullscreen( bool fs ) { _win->fullscreen( fs ); }

    /// @brief Move the window.
    ///
    /// Move the window to the given position.
    /// @param x New x position of the window
    /// @param y New y position of the window
    void move( coord_type x, coord_type y ) { _win->move( x, y ); }
    void move( const point &p ) { move( p[0], p[1] ); }

    /// @brief Resize the window.
    ///
    /// Resize the window to the given size.
    /// @param w New width of the window
    /// @param h New height of the window
    void resize( coord_type w, coord_type h ) { _win->resize( w, h ); }
    void resize( const size &s ) { resize( s.w(), s.h() ); }

    /// @brief Set minimum window size.
    ///
    /// The window will not be allowed to resize smaller than the minimum given.
    /// @param w Minimum width for the window
    /// @param h Minimum height for the window
    void set_minimum_size( coord_type w, coord_type h )
    {
        _win->set_minimum_size( w, h );
    }

    /// @brief Set the window title.
    ///
    /// Set the window title shown in the title bar.
    /// @param t The window title
    void set_title( const std::string &t ) { _win->set_title( t ); }

    /// @brief trigger the window rectangle to be re-drawn
    void invalidate( const rect &r ) { _win->invalidate( r ); }

    //	virtual void set_icon( const icon &i );

    coord_type width( void ) { return _win->width(); }
    coord_type height( void ) { return _win->height(); }

    /// @brief Generic event handler
    ///
    /// This is the most general event handler, in that all events
    /// come in here. Uses of a full-fledged gui widget system
    /// probably want to use this, and dispatch their own versions of
    /// the below, and see all the events.
    std::function<bool( const event & )> event_handoff;

    /// @brief Action for mouse press events.
    ///
    /// Callback action for mouse button press events.
    std::function<void( event_source &, const point &, int )> mouse_pressed;

    /// @brief Action for mouse release events.
    ///
    /// Callback action for mouse button release events.
    std::function<void( event_source &, const point &, int )> mouse_released;

    /// @brief Actionfor mouse motion events.
    ///
    /// Callback action for mouse motion events.
    std::function<void( event_source &, const point & )> mouse_moved;

    /// @brief Action for mouse wheel events.
    ///
    /// Callback action for mouse wheel events.
    std::function<void( event_source &, int )> mouse_wheel;

    /// @brief Action for key press events.
    ///
    /// Callback action for key press events.
    std::function<void( event_source &, scancode )> key_pressed;

    /// @brief Action for key release events.
    ///
    /// Callback action for key release events.
    std::function<void( event_source &, scancode )> key_released;

    /// @brief Action for text entered events.
    ///
    /// Callback action for text entered events.
    std::function<void( event_source &, char32_t )> text_entered;

    std::function<bool( bool )>                   closed;
    std::function<void( void )>                   shown;
    std::function<void( void )>                   hidden;
    std::function<void( void )>                   minimized;
    std::function<void( void )>                   maximized;
    std::function<void( void )>                   restored;
    std::function<void( void )>                   exposed;
    std::function<void( coord_type, coord_type )> moved;
    std::function<void( coord_type, coord_type )> resized;
    std::function<void( void )>                   entered;
    std::function<void( void )>                   exited;

protected:
    bool process_event( const event &e );

private:
    std::shared_ptr<window> _win;
};

} // namespace platform
