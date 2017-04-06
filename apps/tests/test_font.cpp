//
// Copyright (c) 2014-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <fstream>
#include <script/font_manager.h>
#include <base/contract.h>

namespace
{

int safemain( int /*argc*/, char * /*argv*/ [] )
{
	std::shared_ptr<script::font_manager> fontmgr = script::font_manager::common();

	auto fams = fontmgr->get_families();
	for ( auto f: fams )
		std::cout << f << std::endl;

	auto font = fontmgr->get_font( "Times", "bold", 16 );

	if ( font )
	{
		for ( char32_t c = 'a'; c <= 'z'; ++c )
			font->load_glyph( c );

		std::ofstream out( "font.raw" );
		auto bmp = font->bitmap();
		out.write( reinterpret_cast<const char *>( bmp.data() ), static_cast<std::streamsize>( bmp.size() ) );
	}
	else
		throw std::runtime_error( "could not load font" );

	return 0;
}

}

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

