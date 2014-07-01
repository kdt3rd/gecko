
#include <fstream>
#include <script/fontconfig/font_manager.h>
#include <base/contract.h>

namespace
{

int safemain( int argc, char *argv[] )
{
	script::fontconfig::font_manager fontmgr;

	auto fams = fontmgr.get_families();
	for ( auto f: fams )
		std::cout << f << std::endl;

	auto font = fontmgr.get_font( "Times", "bold", 16 );

	for ( char32_t c = 'a'; c <= 'z'; ++c )
		font->load_glyph( c );

	std::ofstream out( "font.raw" );
	auto bmp = font->bitmap();
	out.write( reinterpret_cast<const char *>( bmp.data() ), bmp.size() );

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

