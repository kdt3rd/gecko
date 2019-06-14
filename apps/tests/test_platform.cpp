// Copyright (c) 2014-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include <base/contract.h>
#include <gl/opengl.h>
#include <platform/dispatcher.h>
#include <platform/platform.h>
#include <platform/simple_window.h>
#include <platform/system.h>

namespace
{
int safemain( int /*argc*/, char * /*argv*/[] )
{
    auto sys = platform::platform::find_running();

    auto screens = sys->screens();

    for ( auto scr: screens )
        std::cout << "Screen full " << scr->bounds( false ) << " active "
                  << scr->bounds( true ) << " dpi " << scr->dpi() << " refresh "
                  << scr->refresh_rate()
                  << ( scr->is_default() ? " DEFAULT" : " ALT" ) << std::endl;

    auto win = std::make_shared<platform::simple_window>( sys->new_window() );
    win->set_title( "Hello World" );
    win->exposed = [&]( void ) {
        glViewport(
            0,
            0,
            static_cast<GLsizei>( win->width() ),
            static_cast<GLsizei>( win->height() ) );
        glClearColor( 0.0, 1.0, 0.0, 1.0 );
        glClear( GL_COLOR_BUFFER_BIT );
    };
    win->resized = [&]( double w, double h ) {
        std::cout << "Resized to: " << w << 'x' << h << std::endl;
    };

    win->show();

    auto dispatch = sys->get_dispatcher();
    return dispatch->execute();
    ;
}

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
        base::print_exception( std::cerr, e );
    }
    return ret;
}

////////////////////////////////////////
