// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include <base/contract.h>
#include <draw/icons.h>
#include <functional>
#include <gui/application.h>
#include <gui/button.h>
#include <gui/checkbox.h>
#include <gui/choices.h>
#include <gui/container.h>
#include <gui/label.h>
#include <gui/line_edit.h>
#include <gui/scroll_area.h>
#include <gui/scroll_bar.h>
#include <gui/slider.h>
#include <gui/window.h>
#include <iostream>
#include <memory>
#include <platform/platform.h>
#include <random>
#include <sstream>

namespace
{
static std::shared_ptr<gui::application> app;

////////////////////////////////////////

int safemain( int /* argc */, char ** /* argv */ )
{
    app = std::make_shared<gui::application>();
    app->push();

    auto win = app->new_window();
    win->set_default_cursor(
        app->builtin_cursor( gui::standard_cursor::DEFAULT ) );
    win->set_title( app->active_platform() );

    win->in_context( [&]( void ) {
        using namespace gui;

        auto fakemenu = box( alignment::LEFT );
        fakemenu->set_padding(
            0, 0, win->from_native_vert( 2 ), win->from_native_vert( 2 ) );
        fakemenu->set_spacing(
            win->from_native_horiz( 8 ), win->from_native_vert( 8 ) );
        fakemenu->add( label( "File", alignment::LEFT ) );
        fakemenu->add( label( "Edit", alignment::LEFT ) );
        fakemenu->add( label( "Help", alignment::LEFT ) );
        auto filler = label( "", alignment::LEFT );
        filler->layout_target()->set_expansion_priority( 1 );
        fakemenu->add( filler );

        auto l    = label( "Hello World", alignment::LEFT );
        auto b    = button( "Click Me" );
        auto sl   = slider();
        auto cbox = checkbox();
        auto ch   = choices();
        ch->add_choice( "Choice 1" );
        ch->add_choice( "Choice 2" );
        ch->add_choice( "Choice 3" );
        ch->when_activated.connect( [&]( size_t c ) {
            win->pop_cursor();
            switch ( c )
            {
                case 0:
                    win->push_cursor( app->builtin_cursor(
                        gui::standard_cursor::CROSSHAIR ) );
                    break;
                case 1:
                    win->push_cursor( app->builtin_cursor(
                        gui::standard_cursor::RESIZE_ROW ) );
                    break;
                case 2:
                    win->push_cursor(
                        app->builtin_cursor( gui::standard_cursor::TEXT ) );
                    break;
                default: break;
            }
        } );
        auto ledit  = line_edit();
        auto sbar   = scroll_bar();
        auto sarea  = scroll_area();
        auto center = label(
            "Scroll!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!END",
            alignment::CENTER );

        sarea->set_widget( center );
        sarea->layout_target()->set_expansion_flex( 1.0 );

        auto bx = box( alignment::BOTTOM );
        bx->set_padding(
            win->from_native_horiz( 12 ),
            win->from_native_horiz( 12 ),
            win->from_native_vert( 5 ),
            win->from_native_vert( 5 ) );
        bx->set_spacing(
            win->from_native_horiz( 8 ), win->from_native_vert( 2 ) );

        bx->add( fakemenu );
        bx->add( l );
        size_t n = bx->add( b );
        b->when_activated.connect( [=]( void ) {
            bx->remove( n );
            bx->remove( n + 1 );
        } );
        bx->add( sl );
        bx->add( cbox );
        bx->add( ch );
        bx->add( ledit );
        bx->add( sbar );
        bx->add( sarea );

        win->set_widget( bx );
    } );

    win->show();
    int code = app->run();
    app->pop();
    app.reset();

    return code;
}

////////////////////////////////////////

} // namespace

////////////////////////////////////////

int main( int argc, char *argv[] )
{
    int ret = -1;
    try
    {
        ret = safemain( argc, argv );
    }
    catch ( std::exception &e )
    {
        if ( app )
        {
            app->pop();
            app.reset();
        }
        base::print_exception( std::cerr, e );
    }
    return ret;
}

////////////////////////////////////////
