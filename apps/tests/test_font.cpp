// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include <base/contract.h>
#include <fstream>
#include <script/font_manager.h>

namespace
{
int safemain( int /*argc*/, char * /*argv*/[] )
{
    std::shared_ptr<script::font_manager> fontmgr =
        script::font_manager::make();

    auto fams = fontmgr->get_families();
    for ( auto f: fams )
        std::cout << f << std::endl;

    auto font = fontmgr->get_font( "Times", "bold", 16, 95, 95, 1024, 1024 );

    if ( font )
    {
        for ( char32_t c = 'a'; c <= 'z'; ++c )
            font->load_glyph( c );

        std::ofstream out( "font.raw" );
        auto          bmp = font->bitmap();
        out.write(
            reinterpret_cast<const char *>( bmp.data() ),
            static_cast<std::streamsize>( bmp.size() ) );
    }
    else
        throw std::runtime_error( "could not load font" );

    return 0;
}

} // namespace

////////////////////////////////////////

int main( int argc, char *argv[] )
{
    try
    {
        return safemain( argc, argv );
    }
    catch ( std::exception &e )
    {
        base::print_exception( std::cerr, e );
    }
    return -1;
}
