// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include <draw/colors.h>
#include <gui/application.h>
#include <gui/container.h>
#include <gui/label.h>
#include <gui/window.h>

namespace
{
int safemain( int /*argc*/, char * /*argv*/[] )
{
    auto app = std::make_shared<gui::application>();
    app->push();

    auto win = app->new_window();
    win->set_title( "Color Test" );

    // Names of color palettes
    auto &names = draw::palette_names();

    // Create a grid of colors
    auto grid = gui::grid();
    grid->layout().add_columns( 11 );
    //	grid->layout().add_columns( 1, 0.0 );
    //	grid->layout().add_columns( 10, 0.0 );
    grid->layout().add_rows( names.size() );
    grid->set_padding( 15, 15, 15, 15 );
    grid->set_spacing( 5, 5 );

    for ( size_t n = 0; n < names.size(); ++n )
    {
        grid->add( gui::label( names[n] ), 0, n );
        auto &pal = draw::get_palette( names[n] );
        if ( n == 0 )
        {
            for ( size_t i = 0; i < pal.size(); ++i )
            {
                float r, g, b;
                pal[i].get_lin( r, g, b );
                float l = pal[i].relative_luminance();
                std::cout << i << ' ' << l << ' ' << r << ' ' << g << ' ' << b
                          << '\n';
            }
        }
        for ( size_t i = 0; i < pal.size(); ++i )
        {
            auto swatch = gui::label();
            swatch->set_bg( pal[i] );
            swatch->layout_target()->set_minimum( 24, 24 );
            swatch->layout_target()->set_expansion_flex( 1.0 );
            grid->add( swatch, i + 1, n );
        }
    }

    win->set_widget( grid );
    win->show();

    int result = app->run();
    app->pop();
    app.reset();
    return result;
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
